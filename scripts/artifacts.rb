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
#   ./scripts/artifacts.rb -v -c web
#
#   # Publish artifact folders to itch.io.
#   ./scripts/artifacts.rb -I -c lin
#   ./scripts/artifacts.rb -I -c mac
#   ./scripts/artifacts.rb -I -c win
#   ./scripts/artifacts.rb -I -c web
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
  VERSION = '0.1.8'

  ARTIFACTS_DIR = File.join('build','artifacts')
  USER_GAME = 'esotericpig/ekoscape'

  ARTIFACTS = [
    {
      channel: 'linux-x64',
      name: 'linux-appimage-x64',
      file: 'EkoScape-linux-x64.tar.gz',
    },
    {
      channel: 'macos-universal',
      name: 'macos-uni',
      file: 'EkoScape-macos-universal.tar.gz',
    },
    {
      channel: 'windows-x64',
      name: 'windows-x64',
      file: 'EkoScape-windows-x64.zip',
    },
    {
      channel: 'web',
      dir: 'bin_web/Release',
      ignores: %w[ekoscape.html], # Copied as `index.html`. See `CMakeLists.txt` for explanation.
    },
  ].freeze

  SLEEP_SECS = 0.500
  CHECKSUM_BUFFER_SIZE = 16 * 1024

  GH_CMD = %w[ gh ].freeze
  TAR_CMD = %w[ tar ].freeze
  UNZIP_CMD = %w[ unzip ].freeze
  BUTLER_CMD = %w[ butler ].freeze

  # Order matters! Because user can specify all actions.
  ACTIONS = [
    [:C,"[clean] delete '#{ARTIFACTS_DIR}/'",:clean],
    [:g,"[get] download artifacts to '#{ARTIFACTS_DIR}/'",:fetch],
    [:k,'[checksum] verify artifact checksums',:check],
    [:x,'[extract] extract artifacts',:extract],
    [:v,'[validate] validate artifacts for itch.io',:validate_for_itch],
    [:I,'[itch] publish extracted artifacts to itch.io',:publish_to_itch],
    [:s,'[status] check status of itch.io builds',:stat_itch_builds],
  ].map do |action|
    {opt: action[0],desc: action[1],method: action[2]}
  end.freeze

  def initialize
    @artifacts = ARTIFACTS.map do |artifact|
      artifact[:file] = File.join(ARTIFACTS_DIR,artifact[:file]) unless artifact[:file].nil?
      artifact[:dest_dir] = File.join(ARTIFACTS_DIR,artifact[:dest_dir]) unless artifact[:dest_dir].nil?

      Artifact.new(**artifact)
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
      op.separator 'Channels'
      @artifacts.each { |artifact| op.separator "#{si}#{artifact.channel}" }

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
      next :skip if artifact.name.nil?

      run_cmd(GH_CMD,'run','download','--dir',ARTIFACTS_DIR,'--name',artifact.name)
    end

    check
  end

  def check
    each_artifact(pauses: false,newlines: false) do |artifact|
      sum_file = "#{artifact.file}.sha256"

      next :skip unless File.file?(sum_file)

      verify_checksum_file(sum_file)
    end
  end

  def extract
    each_artifact(show_result: true) do |artifact|
      next :skip if artifact.file.nil?

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
      artifact.ignores.each { |ignore| cmd.push('--ignore',ignore) }
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
      next if result == :skip

      if show_result
        puts if newlines
        puts "=> Channel [#{artifact.channel}] succeeded!" if result
      end
      abort "=> Channel [#{artifact.channel}] failed!" unless result

      sleep(SLEEP_SECS) if pauses
      puts if newlines
    end

    puts "=> All channels succeeded! [#{@artifacts.map(&:channel).join(',')}]" if show_result
  end

  def verify_checksum_file(sum_file)
    raise "Invalid checksum file [#{sum_file}]." if sum_file.empty? || !File.file?(sum_file)

    result = true

    File.foreach(sum_file,mode: 'rt',encoding: 'BOM|UTF-8:UTF-8') do |line|
      parts = line.strip.split(/\s+\*?/,2)

      next if parts.length < 2

      hex = parts[0].strip
      file = parts[1].strip

      next if hex.empty? || file.empty?

      file = File.join(File.dirname(sum_file),file)
      result &&= verify_checksum(file,hex)
    end

    return result
  end

  def verify_checksum(file,hex)
    hex = hex.strip

    result = false
    summary = ''
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
        result = true
        summary = '[ok]'
      else
        result = false
        summary = '[BAD hex]'
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
      result = false
      summary = '[NO file]'
    end

    fmt = '%-9s %s'
    puts format(fmt,summary,file)
    puts details.map { |d| format(fmt,'',d) }.join("\n") unless details.nil?

    return result
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

    return run_cmd(cmd)
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
  attr_reader :channel
  attr_reader :name
  attr_reader :file
  attr_reader :dest_dir
  attr_reader :ignores
  attr_reader :platform
  attr_reader :arch

  def initialize(channel:,name: nil,file: nil,dir: nil,dest_dir: :parse,ignores: [],platform: :parse,
                 arch: :parse)
    file = nil if (file = file&.strip)&.empty?
    dir = nil if (dir = dir&.strip)&.empty?

    raise 'Must specify either file or dir.' if file.nil? && dir.nil?

    dest_dir = dir if file.nil?

    if dest_dir == :parse
      dest_dir = file.nil? ? '' : file.sub(/([^.])\..*$/,'\1').strip

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

    @channel = channel.strip
    @name = name&.strip
    @file = file
    @dest_dir = dest_dir
    @ignores = ignores.map(&:strip).reject(&:empty?)
    @platform = platform&.strip
    @arch = arch&.strip
  end

  def inspect
    s = ''.dup

    s << @channel.inspect << ': {'
    s << @name.inspect
    s << ', ' << @file.inspect
    s << ', ' << @dest_dir.inspect
    s << ', ' << @ignores.inspect
    s << ', ' << @platform.inspect
    s << ', ' << @arch.inspect
    s << '}'

    return s
  end

  def to_s
    return inspect
  end
end

main if __FILE__ == $PROGRAM_NAME
