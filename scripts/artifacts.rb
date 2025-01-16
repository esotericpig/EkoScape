#!/usr/bin/env ruby
# encoding: UTF-8
# frozen_string_literal: true

###
# Show usage:
#   ./scripts/artifacts.rb
#
# Example usage:
#   # Use `-n` to only perform a dry-run.
#   # Use `-c <channel>` to filter which artifacts/channels to use (fuzzy search).
#
#   # Download GitHub artifacts to `build/artifacts/`.
#   ./scripts/artifacts.rb -g
#
#   # Extract (decompress) artifacts to `build/artifacts/`.
#   ./scripts/artifacts.rb -x
#
#   # Validate artifact folders for itch.io.
#   ./scripts/artifacts.rb -v -c lin
#   ./scripts/artifacts.rb -v -c mac
#   ./scripts/artifacts.rb -v -c win
#
#   # Publish artifact folders to itch.io.
#   ./scripts/artifacts.rb -I -c lin
#   ./scripts/artifacts.rb -I -c mac
#   ./scripts/artifacts.rb -I -c win
#
#   # Check status of itch.io builds.
#   ./scripts/artifacts.rb -s
#
# @author Bradley Whited
###

require 'digest'
require 'fileutils'
require 'optparse'
require 'set'
require 'shellwords'

def main
  ArtifactsMan.new.run
end

class ArtifactsMan
  VERSION = '0.1.3'

  ARTIFACTS_DIR = File.join('build','artifacts')
  USER_GAME = 'esotericpig/ekoscape'

  ARTIFACTS = [
    {
      name: 'linux-appimage-x64',
      file: 'EkoScape-linux-x64.tar.gz',
      channel: 'linux-x64',
    },
    {
      name: 'macos-uni',
      file: 'EkoScape-macos-universal.tar.gz',
      channel: 'macos-universal',
    },
    {
      name: 'windows-x64',
      file: 'EkoScape-windows-x64.zip',
      channel: 'windows-x64',
    },
  ].freeze

  SLEEP_SECS = 0.500
  CHECKSUM_BUFFER_SIZE = 16_384

  GH_CMD = %w[ gh ].freeze
  TAR_CMD = %w[ tar ].freeze
  UNZIP_CMD = %w[ unzip ].freeze
  BUTLER_CMD = %w[ butler ].freeze

  # Order matters! Because user can specify all actions.
  ACTIONS = [
    [:C,"[clean] delete '#{ARTIFACTS_DIR}/'",:clean],
    [:g,"[get] download artifacts to '#{ARTIFACTS_DIR}/'",:fetch],
    [:h,'[checksum] verify artifact checksums',:check],
    [:x,'[extract] extract artifacts',:extract],
    [:v,'[validate] validate artifacts for itch.io',:validate_for_itch],
    [:I,'[itch] publish extracted artifacts to itch.io',:publish_to_itch],
    [:s,'[status] check status of itch.io builds',:stat_itch_builds],
  ].map do |action|
    {opt: action[0],desc: action[1],method: action[2]}
  end.freeze

  def initialize
    @artifacts = ARTIFACTS.map do |artifact|
      Artifact.new(parent_dir: ARTIFACTS_DIR,**artifact)
    end
    @dry_run = true
    @extra_args = []
  end

  def run
    opts = {}
    op = build_opt_parser(opts)
    args,@extra_args = parse_extra_args

    op.parse!(args,into: opts)

    if opts.empty? || ACTIONS.none? { |action| opts[action[:opt]] }
      puts op.help
      exit
    end

    @dry_run = opts[:n]
    channels = opts[:channel]

    if !channels.nil? && !channels.empty?
      @artifacts.filter! do |artifact|
        channels.any? { |channel| artifact.channel.downcase.include?(channel.strip.downcase) }
      end
    end

    ACTIONS.each do |action|
      method(action[:method]).call if opts[action[:opt]]
    end
  end

  def build_opt_parser(opts)
    return OptionParser.new do |op|
      op.program_name = File.basename($PROGRAM_NAME)
      op.version = VERSION
      op.summary_width = 16

      si = op.summary_indent

      op.separator ''
      op.separator "v#{op.version}"

      op.separator ''
      op.separator 'Options'
      op.on('-c <channel>','filter which artifacts to use (fuzzy search)') do |channel|
        c = opts.fetch(:channel) { |key| opts[key] = Set.new }
        c << channel
        c
      end

      op.separator ''
      op.separator 'Actions'
      ACTIONS.each { |action| op.on("-#{action[:opt]}",nil,action[:desc]) }

      op.separator ''
      op.separator 'Basic Options'
      op.on('-n',nil,'no-clobber dry run')

      op.separator ''
      op.separator 'Notes'
      op.separator "#{si}# Any trailing options/args after '--' will be passed to the command directly:"
      op.separator "#{si}#{op.program_name} -v -c lin -- --context-timeout=110"
    end
  end

  def parse_extra_args(args=ARGV)
    dash_i = args.find_index('--')

    if dash_i.nil?
      extra_args = []
    else
      extra_args = args[dash_i + 1..]
      args = args[0...dash_i]
    end

    return [args,extra_args]
  end

  def clean
    if File.directory?(ARTIFACTS_DIR)
      FileUtils.rm_r(ARTIFACTS_DIR,noop: @dry_run,verbose: true)
    else
      puts "Already gone: #{ARTIFACTS_DIR}"
    end
  end

  def fetch
    FileUtils.mkdir_p(ARTIFACTS_DIR,noop: @dry_run,verbose: true) unless File.directory?(ARTIFACTS_DIR)

    # NOTE: Must download each one separately so that it doesn't create subdirs.
    each_artifact do |artifact|
      run_cmd(GH_CMD,'run','download','--dir',ARTIFACTS_DIR,'--name',artifact.name)
    end

    check
  end

  def check
    each_artifact(pauses: false,newlines: false) do |artifact|
      sum_file = "#{artifact.file}.sha256"

      next unless File.file?(sum_file)

      verify_checksum_file(sum_file)
    end
  end

  def extract
    each_artifact(show_result: true) do |artifact|
      extract_file(artifact.file,dest_dir: artifact.dest_dir)
    end
  end

  def validate_for_itch
    each_artifact(show_result: true) do |artifact|
      cmd = [BUTLER_CMD,'validate']
      cmd.push('--platform',artifact.platform) unless artifact.platform.nil?
      cmd.push('--arch',artifact.arch) unless artifact.arch.nil?
      cmd.push(artifact.dest_dir)

      run_cmd(cmd)
    end
  end

  def publish_to_itch
    each_artifact do |artifact|
      cmd = [BUTLER_CMD,%w[ push --fix-permissions --dereference --if-changed ]]
      cmd.push('--dry-run') if @dry_run
      cmd.push(artifact.dest_dir,"#{USER_GAME}:#{artifact.channel}")

      run_cmd(cmd,dry_run: false) # Butler has own dry-run.
    end
  end

  def stat_itch_builds
    if @artifacts.length == ARTIFACTS.length
      run_cmd(BUTLER_CMD,'status',USER_GAME)
      puts
      return
    end

    each_artifact do |artifact|
      run_cmd(BUTLER_CMD,'status',"#{USER_GAME}:#{artifact.channel}")
    end
  end

  def each_artifact(pauses: true,newlines: true,show_result: false)
    if @dry_run
      pauses = false
      show_result = false
    end

    @artifacts.each do |artifact|
      result = yield artifact

      if show_result
        puts if newlines

        if result
          puts "=> Channel [#{artifact.channel}] succeeded!"
        else
          abort "=> Channel [#{artifact.channel}] failed!"
        end
      end

      sleep(SLEEP_SECS) if pauses
      puts if newlines
    end

    puts "=> All channels succeeded! [#{@artifacts.map(&:channel).join(',')}]" if show_result
  end

  def verify_checksum_file(sum_file)
    raise "Invalid checksum file [#{sum_file}]." if sum_file.empty? || !File.file?(sum_file)

    File.foreach(sum_file,mode: 'rt',encoding: 'BOM|UTF-8:UTF-8') do |line|
      parts = line.strip.split(/\s+\*?/,2)

      next if parts.length < 2

      hex = parts[0].strip
      file = parts[1].strip

      next if hex.empty? || file.empty?

      file = File.join(File.dirname(sum_file),file)

      verify_checksum(file,hex)
    end
  end

  def verify_checksum(file,hex)
    hex = hex.strip

    result = ''
    details = nil

    if !file.empty? && File.file?(file)
      dig = Digest::SHA256.new

      if @dry_run
        actual_hex = hex
      else
        File.open(file,'rb') do |f|
          buffer = ''.dup

          while f.read(CHECKSUM_BUFFER_SIZE,buffer)
            dig.update(buffer)
          end
        end

        actual_hex = dig.hexdigest
      end

      if actual_hex == hex
        result = '[ok]'
      else
        result = '[BAD hex]'
        diff = Array.new((hex.length >= actual_hex.length) ? hex.length : actual_hex.length)

        (0..diff.length).each do |i|
          diff[i] = (actual_hex[i] == hex[i]) ? ' ' : '^'
        end

        details = [
          "expected: #{hex}",
          "actual:   #{actual_hex}",
          "diff:     #{diff.join}",
        ]
      end
    else
      result = '[NO file]'
    end

    fmt = '%-9s %s'
    puts format(fmt,result,file)
    puts details.map { |d| format(fmt,'',d) }.join("\n") unless details.nil?
  end

  def extract_file(file,dest_dir: nil)
    raise 'Empty file.' if (file = file.strip).empty?
    dest_dir = nil if !dest_dir.nil? && (dest_dir = dest_dir.strip).empty?

    cmd = []

    # Can't use File.extname() because of `.tar.gz` (double).
    case file
    when /.tar.gz$/
      cmd.concat(TAR_CMD)
      cmd.push('-xzf',file,'--keep-old-files')
      cmd.push('-C',dest_dir) unless dest_dir.nil?
    when /.zip$/
      cmd.concat(UNZIP_CMD)
      cmd.push('-n',file)
      cmd.push('-d',dest_dir) unless dest_dir.nil?
    else
      raise "Invalid file type to extract: #{file}."
    end

    FileUtils.mkdir(dest_dir,noop: @dry_run,verbose: true) if !dest_dir.nil? && !File.directory?(dest_dir)
    run_cmd(cmd)
  end

  def run_cmd(*cmd,dry_run: @dry_run)
    cmd += @extra_args
    cmd = cmd.flatten.compact.map(&:to_s)

    puts cmd.map(&Shellwords.method(:escape)).join(' ')

    return true if dry_run
    return system(*cmd)
  end
end

class Artifact
  attr_reader :name,:file,:dest_dir,:channel,:platform,:arch

  def initialize(name:,file:,channel:,parent_dir: nil,dest_dir: nil,platform: :parse,arch: :parse)
    file = file.strip

    if dest_dir.nil? || (dest_dir = dest_dir.to_s.strip).empty?
      dest_dir = file.sub(/([^.])\..*$/,'\1')

      raise "Invalid file/ext: #{file}." if dest_dir.empty?
    end
    if platform == :parse
      # Channel can have multiple platforms.
      platforms = []

      # See: https://itch.io/docs/butler/pushing.html#channel-names
      platforms << 'windows' if channel =~ /win|windows/i
      platforms << 'linux' if channel =~ /linux/i
      platforms << 'osx' if channel =~ /mac|osx/i

      platform = (platforms.size == 1) ? platforms[0] : nil
    end
    if arch == :parse
      # Channel can have multiple architectures.
      arches = []

      arches << '386' if channel =~ /386|686|x86[^_-]|32/i
      arches << 'amd64' if channel =~ /amd64|x86[_-]64|64/i

      arch = (arches.size == 1) ? arches[0] : nil
    end

    @name = name.strip
    @channel = channel.strip
    @platform = platform&.strip
    @arch = arch&.strip

    if parent_dir.nil?
      @file = file
      @dest_dir = dest_dir
    else
      @file = File.join(parent_dir,file)
      @dest_dir = File.join(parent_dir,dest_dir)
    end
  end

  def inspect
    return "{#{@name.inspect},#{@file.inspect},#{@dest_dir.inspect}" \
           ",#{@channel.inspect},#{@platform.inspect},#{@arch.inspect}}"
  end

  def to_s
    return inspect
  end
end

main if __FILE__ == $PROGRAM_NAME
