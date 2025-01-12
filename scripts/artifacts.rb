#!/usr/bin/env ruby
# encoding: UTF-8
# frozen_string_literal: true

###
# Show usage:
#   $ ./artifacts.rb
#
# @version 0.1.1
# @author Bradley Whited
###

require 'digest'
require 'fileutils'
require 'optparse'
require 'shellwords'

def main
  ArtifactsMan.new.run
end

Artifact = Struct.new(:name,:file,:to_dir,:channel,keyword_init: true) do
  def initialize(name:,file:,channel:,to_dir: nil)
    if to_dir.nil? || (to_dir = to_dir.to_s.strip).empty?
      to_dir = file.sub(/([^.])\..*$/,'\1')

      raise "Invalid file/ext: #{file}." if to_dir.empty?
    end

    self.name = name.strip
    self.file = file.strip
    self.to_dir = to_dir.strip
    self.channel = channel.strip
  end
end

class ArtifactsMan
  ARTIFACTS_DIR = File.join('build','artifacts')
  USER_GAME = 'esotericpig/ekoscape'
  SLEEP_SECS = 0.500

  ARTIFACTS = [
    Artifact.new(
      name: 'linux-appimage-x64',
      file: 'EkoScape-linux-x64.tar.gz',
      channel: 'linux-x64',
    ),
    Artifact.new(
      name: 'macos-uni',
      file: 'EkoScape-macos-universal.tar.gz',
      channel: 'macos-universal',
    ),
    Artifact.new(
      name: 'windows-x64',
      file: 'EkoScape-windows-x64.zip',
      channel: 'windows-x64',
    ),
  ].freeze

  GH_CMD = %w[ gh ].freeze
  TAR_CMD = %w[ tar ].freeze
  UNZIP_CMD = %w[ unzip ].freeze
  BUTLER_CMD = %w[ butler ].freeze

  def initialize
    @artifacts = ARTIFACTS
    @dry_run = true
    @extra_args = []
  end

  def run
    opt_parser = OptionParser.new do |op|
      op.program_name = File.basename($PROGRAM_NAME)
      op.version = '0.1.1'
      op.summary_width = 16

      op.separator ''
      op.separator "v#{op.version}"

      op.separator ''
      op.separator 'Options'
      op.on('-c <channel>','filter which artifacts to use by fuzzy search on channel') do |channel|
        channel = channel.strip
        @artifacts = @artifacts.filter { |a| a.channel.include?(channel) }
        channel
      end
      op.separator ''
      op.on('-C',nil,"[clean] delete '#{ARTIFACTS_DIR}/'")
      op.on('-g',nil,"[get] download artifacts to '#{ARTIFACTS_DIR}/'")
      op.on('-h',nil,'[checksum] verify artifact checksums')
      op.on('-x',nil,'[extract] extract artifacts')
      op.on('-v',nil,'[validate] validate artifacts for itch.io')
      op.on('-I',nil,'[itch] publish extracted artifacts to itch.io')
      op.on('-s',nil,'[status] check status of itch.io builds')

      op.separator ''
      op.separator 'Basic Options'
      op.on('-n',nil,'no-clobber dry run')
    end

    args,@extra_args = parse_extra_args
    opts = {}
    opt_parser.parse!(args,into: opts)

    if opts.empty?
      puts opt_parser.help
      exit
    end

    @dry_run = opts[:n]

    # Order matters! Because user can specify all opts.
    clean if opts[:C]
    fetch if opts[:g]
    check if opts[:h]
    extract if opts[:x]
    validate_for_itch if opts[:v]
    publish_to_itch if opts[:I]
    stat_itch_builds if opts[:s]
  end

  def parse_extra_args(args=ARGV)
    dash_i = args.find_index('--')
    return [args,[]] if dash_i.nil?

    extra_args = args[dash_i + 1..]
    args = args[0...dash_i]

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
    @artifacts.each do |artifact|
      run_cmd(GH_CMD,'run','download','--dir',ARTIFACTS_DIR,'--name',artifact.name)
      sleep(SLEEP_SECS)
      puts
    end

    check
  end

  def check
    @artifacts.each do |artifact|
      file = File.join(ARTIFACTS_DIR,artifact.file)

      check_file("#{file}.sha256")
    end
  end

  def extract
    @artifacts.each do |artifact|
      file = File.join(ARTIFACTS_DIR,artifact.file)
      to_dir = File.join(ARTIFACTS_DIR,artifact.to_dir)

      extract_file(file,to_dir: to_dir)
      sleep(SLEEP_SECS)
      puts
    end
  end

  def validate_for_itch
    @artifacts.each do |artifact|
      dir = File.join(ARTIFACTS_DIR,artifact.to_dir,'')

      run_cmd(BUTLER_CMD,'validate',dir)
      sleep(SLEEP_SECS)
      puts
    end
  end

  def publish_to_itch
    @artifacts.each do |artifact|
      dir = File.join(ARTIFACTS_DIR,artifact.to_dir,'')

      cmd = [BUTLER_CMD,%w[ push --fix-permissions --dereference --if-changed ]]
      cmd.push('--dry-run') if @dry_run
      cmd.push(dir,"#{USER_GAME}:#{artifact.channel}")

      run_cmd(cmd,dry_run: false) # Butler has own dry-run.
      sleep(SLEEP_SECS)
      puts
    end
  end

  def stat_itch_builds
    if @artifacts.length == ARTIFACTS.length
      run_cmd(BUTLER_CMD,'status',USER_GAME)
      puts
      return
    end

    @artifacts.each do |artifact|
      run_cmd(BUTLER_CMD,'status',"#{USER_GAME}:#{artifact.channel}")
      sleep(SLEEP_SECS)
      puts
    end
  end

  def check_file(sum_file)
    File.foreach(sum_file,mode: 'rt',encoding: 'BOM|UTF-8:UTF-8') do |line|
      parts = line.strip.split(/\s+\*?/,2)
      next if parts.length < 2

      sum = parts[0].strip
      file = parts[1].strip
      next if sum.empty? || file.empty?

      file = File.join(ARTIFACTS_DIR,file)
      dig = Digest::SHA256.new

      File.open(file,'rb') do |f|
        buffer = ''.dup
        while f.read(16_384,buffer)
          dig.update(buffer)
        end
      end

      print (dig.hexdigest == sum) ? '[ok]' : '[BAD]'
      puts " #{file}"
    end
  end

  def extract_file(file,to_dir: nil)
    raise 'Empty file.' if (file = file.strip).empty?
    to_dir = nil if !to_dir.nil? && (to_dir = to_dir.strip).empty?

    cmd = []

    # Can't use File.extname() because of `.tar.gz` (double).
    case file
    when /.tar.gz$/
      cmd.concat(TAR_CMD)
      cmd.push('-xzf',file,'--keep-old-files')
      cmd.push('-C',to_dir) unless to_dir.nil?
    when /.zip$/
      cmd.concat(UNZIP_CMD)
      cmd.push('-n',file)
      cmd.push('-d',to_dir) unless to_dir.nil?
    else
      raise "Invalid file type to extract: #{file}."
    end

    FileUtils.mkdir(to_dir,noop: @dry_run,verbose: true) if !to_dir.nil? && !File.directory?(to_dir)
    run_cmd(cmd)
  end

  def run_cmd(*cmd,dry_run: @dry_run)
    cmd += @extra_args
    cmd = cmd.flatten.compact.map(&:to_s)

    puts cmd.map(&Shellwords.method(:escape)).join(' ')
    system(*cmd) unless dry_run
  end
end

main if __FILE__ == $PROGRAM_NAME
