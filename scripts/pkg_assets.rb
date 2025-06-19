#!/usr/bin/env ruby
# encoding: UTF-8
# frozen_string_literal: true

###
# Show usage:
#   ./scripts/pkg_assets.rb
#
# Example usage:
#   # -C    clean `pkgs/assets.zip`
#   # -p    package up `assets/` into `pkgs/assets.zip`
#   # -G    upload package to GitHub (overwrites package)
#   ./scripts/pkg_assets.rb -C -p -G
#
# @author Bradley Whited
###

require 'fileutils'
require 'optparse'
require 'shellwords'

def main
  AssetsPkger.new.run
end

class AssetsPkger
  VERSION = '0.1.4'

  DEST_DIR = File.join('pkgs')
  ASSETS_NAME = 'assets'
  ASSETS_DIR = File.join(ASSETS_NAME)
  ASSETS_PKG = File.join(DEST_DIR,"#{ASSETS_NAME}.zip")

  ZIP_CMD = %w[zip -r -9 -v -y].freeze
  # Excludes must be at end of full command.
  ZIP_CMD_SUFFIX = ['-x','maps/*'].freeze
  GH_CMD = %w[gh release upload --clobber v1.99].freeze

  def initialize
    @dry_run = true
  end

  def run
    opt_parser = OptionParser.new do |op|
      op.program_name = File.basename($PROGRAM_NAME)
      op.version = VERSION
      op.summary_width = 8

      # Opt indent.
      oi = op.summary_indent + (' ' * op.summary_width)

      op.separator ''
      op.separator "v#{op.version}"

      op.separator ''
      op.separator 'Options'
      op.on('-C',nil,"[clean] delete '#{ASSETS_PKG}'")
      op.on('-p',nil,"[pkg] package up '#{ASSETS_DIR}/' into '#{ASSETS_PKG}'" \
                     "\n#{oi}       -- must have `#{ZIP_CMD[0]}` installed")
      op.on('-G',nil,'[gh] upload package to GitHub release (overwrites package)' \
                     "\n#{oi}      -- must have `#{GH_CMD[0]}` installed")

      op.separator ''
      op.separator 'Basic Options'
      op.on('-n',nil,'no-clobber dry run')
    end

    opts = {}
    opt_parser.parse!(into: opts)

    if opts.empty?
      puts opt_parser.help
      exit
    end

    @dry_run = opts[:n]

    # Order matters! Because user can specify all opts.
    clean if opts[:C]
    pkg_assets if opts[:p]
    upload_to_gh if opts[:G]
  end

  def clean
    if File.file?(ASSETS_PKG)
      FileUtils.rm(ASSETS_PKG,noop: @dry_run,verbose: true)
    else
      puts "[gone] '#{ASSETS_PKG}'"
    end
  end

  def pkg_assets
    FileUtils.mkdir(DEST_DIR,noop: @dry_run,verbose: true) unless File.directory?(DEST_DIR)

    # Need absolute path before cd().
    out_file = File.absolute_path(ASSETS_PKG)

    FileUtils.cd(ASSETS_DIR,verbose: true) do
      run_cmd(*ZIP_CMD,out_file,'.',*ZIP_CMD_SUFFIX)
    end
  end

  def upload_to_gh
    run_cmd(*GH_CMD,ASSETS_PKG)
  end

  def run_cmd(*cmd)
    cmd = cmd.flatten.compact.map(&:to_s)

    puts cmd.map { |a| Shellwords.escape(a) }.join(' ')
    system(*cmd) unless @dry_run
  end
end

main if __FILE__ == $PROGRAM_NAME
