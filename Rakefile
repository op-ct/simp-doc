#!/usr/bin/rake -T

ENV['SIMP_INTERNAL_pkg_ignore'] = 'build/rpm_metadata'

require 'ostruct'
require 'rake/clean'
require 'simp/rake'
require 'yaml'
require 'find'

CLEAN.include 'build/rpm_metadata'
CLEAN.include 'linkcheck'

desc 'Munge Prep'
  desc <<-EOM
This task extracts the docs version and release from a simp.spec file
and then writes these values into a local build/rpm_metadata/release
file. The simp.spec file to use is determined as follows:
1) First look for a simp.spec file within a local simp-core git repo.
   The location of that repo can be specified by SIMP_CORE_PATH.
   Otherwise it defaults to '../..', the location suitable when
   simp-docs are checkout out as part of a simp-core ISO build.
2) If a local simp.spec file cannot be found and the SIMP_BRANCH
   environment variable is specified, pull the simp.spec file
   from github for that simp-core branch.
3) Otherwise, pull the simp.spec file from github for the simp-core
   master branch.
EOM
task 'munge:prep' do
  # Defaults
  #
  # Location of the release metadata file which is referenced
  # by lua simp-doc.spec
  rel_file = './build/rpm_metadata/release'
  #
  # Location of the local simp spec file
  if ENV['SIMP_CORE_PATH']
    local_simp_core_path = File.expand_path(ENV['SIMP_CORE_PATH'])
  else
    local_simp_core_path = File.expand_path(File.join(File.dirname(__FILE__), '..', '..'))
  end
  specfile = File.join(local_simp_core_path, 'src', 'assets', 'simp', 'build', 'simp.spec')
  if File.exist?(specfile)
    # Tell python code where simp-core is during the RPM build process.
    # In the RPM build, it builds the source RPM and then installs it
    # in a temporary location. So, the relative relationship is broken.
    ENV['SIMP_CORE_PATH'] = local_simp_core_path
  end

  #
  # Default simp-core git tag/branch to use to pull the simp.spec when
  # a local simp.spec file does not exist.  This ensures we build *something*.
  default_simp_branch = 'master'

  # Default SIMP version should all other mechanisms to determine it fail
  default_simp_version = '6.1.0-0'
  #
  # Default header to be written to release metadata file
  release_content = <<-EOM
# DO NOT CHECK IN MODIFIED VERSIONS OF THIS FILE!
# It is automatically generated by rake munge:prep
version: __VERSION__
release: __RELEASE__
EOM

  tmpspec = Tempfile.new('docspec')

  begin
    # Create the release metadata file
    FileUtils.mkdir_p('./build/rpm_metadata')
    File.open(rel_file,"w") do |fh|
      fh.puts(release_content)
    end

    # If the SIMP_BRANCH set or default specfile does not exist,
    # try to pull it from Github
    # NOTE:  This code is a (more robust) duplicate of code in
    # docs/conflib/get_simp_version.py.  Since that Python code is
    # used to handle error cases in ReadTheDocs, it must NOT be removed.
    simp_branch = ENV['SIMP_BRANCH']
    if simp_branch or !File.exist?(specfile)
      if simp_branch
        spec_url = "https://raw.githubusercontent.com/simp/simp-core/#{simp_branch}/src/assets/simp/build/simp.spec"
        puts "Using #{spec_url}"
      else
        spec_url = "https://raw.githubusercontent.com/simp/simp-core/#{default_simp_branch}/src/assets/simp/build/simp.spec"
        warn("WARNING: No suitable spec file found at #{specfile}, defaulting to #{spec_url}")
      end

      require 'open-uri'
      begin
        open(spec_url) do |specfile|
          tmpspec.write(specfile.read)
        end
      rescue OpenURI::HTTPError => e
        $stderr.puts e.message
        raise("Could not find a valid spec file at #{spec_url}, check your SIMP_BRANCH environment setting!")
      end

      specfile = tmpspec.path
    end

    # Grab the version and release out of whatever spec we have found.
    rpm_metadata = OpenStruct.new
    rpm_metadata.version, rpm_metadata.release  = default_simp_version.split('-')

    if File.exist?(specfile)
      begin
        rpm_metadata = Simp::RPM.new(specfile)
      rescue StandardError
        warn("Could not obtain valid version/release information from #{specfile}, please check for consistency. Defaulting to: '#{default_simp_version}'")
      end
    end

    # Set the version and release in the rel_file
    %x(sed -i s/version:.*/version:#{rpm_metadata.version}/ #{rel_file})
    %x(sed -i s/release:.*/release:#{rpm_metadata.release.split(rpm_metadata.dist).first}/ #{rel_file})
  ensure
    tmpspec.close
    tmpspec.unlink
  end
end

class DocPkg < Simp::Rake::Pkg
  # We need to inject the SCL Python repos for RHEL6 here if necessary
  def define_clean
    task :clean do
      find_erb_files.each do |erb|
        short_name = "#{File.dirname(erb)}/#{File.basename(erb,'.erb')}"
        if File.exist?(short_name)
          rm(short_name)
        end
      end
    end
  end

  def define_pkg_tar
    # First, we need to assemble the documentation.
    # This doesn't work properly under Ruby >= 1.9 and leaves cruft in the directories
    # We can try again when 'puppet strings' hits the ground
    super
  end

  def find_erb_files(dir=@base_dir)
    to_ret = []
    Find.find(dir) do |erb|
      if erb =~ /\.erb$/
        to_ret << erb
      end
    end

    to_ret
  end
end

DocPkg.new( File.dirname( __FILE__ ) ) do |t|
  # Not sure this is right
  t.clean_list << "#{t.base_dir}/html"
  t.clean_list << "#{t.base_dir}/html-single"
  t.clean_list << "#{t.base_dir}/pdf"
  t.clean_list << "#{t.base_dir}/sphinx_cache"
  t.clean_list << "#{t.base_dir}/docs/dynamic"

  t.exclude_list << 'dist'
  # Need to ignore any generated files from ERB's.
  #t.ignore_changes_list += find_erb_files.map{|x| x = "#{File.dirname(x)}/#{File.basename(x,'.erb')}".sub(/^\.\//,'')}

  Dir.glob('build/rake_helpers/*.rake').each do |helper|
    load helper
  end
end

def process_rpm_yaml(rel, simp_version)
  fail("Must pass release to 'process_rpm_yaml'") unless rel

      header = <<-EOM
SIMP #{simp_version} #{rel} External RPMs
-----------------------------------------

This provides a list of RPMs, and their sources, for non-SIMP components that
are required for system functionality and are specific to an installation on a
#{rel} system.

      EOM

  rpm_data = Dir.glob("../../build/yum_data/SIMP*#{rel}*/packages.yaml")

  table = [ header ]
  table << ".. list-table:: SIMP #{simp_version} #{rel} External RPMs"
  table << '   :widths: 20 80'
  table << '   :header-rows: 1'
  table << ''
  table << '   * - RPM Name'
  table << '     - RPM Source'

  data = ['   * - Not Found']
  data << ['     - Unknown']
  unless rpm_data.empty?
    data = YAML.load_file(rpm_data.sort_by{|filename| File.mtime(filename)}.last)
    data = data.values.map{|x| x = "   * - #{x[:rpm_name]}\n     - #{x[:source]}"}
  end

  table += data

  fh = File.open(File.join('docs','user_guide','RPM_Lists',%(External_SIMP_#{simp_version}_#{rel}_RPM_List.rst)),'w')
  fh.puts(table.join("\n"))
  fh.sync
  fh.close
end

## Custom Linting Checks

def lint_files_with_bad_tags
  file_issues = Hash.new()

  files_with_bad_tags = %x(grep -rne ':[[:alpha:]]\\+`[[:alpha:]]' docs).lines

  files_with_bad_tags.each do |line|
    line.strip!

    line_parts = line.split(': ')

    file_name = line_parts.shift
    issue_text = line_parts.join(': ')

    file_issues[file_name] = {
      :issue_type => 'bad_tag',
      :issue_text => issue_text
    }
  end

  file_issues
end

def run_build_cmd(cmd)
  require 'open3'

  puts "== #{cmd}"

  stdout, stderr, status = Open3.capture3(cmd)

  # This message is garbage
  stderr.gsub!(/WARNING: The config value \S+ has type \S+, defaults to \S+\./,'')
  stderr.strip!

  unless status.success?
#require 'pry'; binding.pry
    $stderr.puts( "== ERROR: `#{cmd}` returned '#{status.exitstatus}'",'STDERR:', '-'*80 , stderr, '-'*80 )
    exit(1)
  end

  return status
end

## End Custom Linting Checks

namespace :docs do
  namespace :rpm do
    desc 'Update the RPM lists'
    task :external do
      simp_version = Simp::RPM.get_info('build/simp-doc.spec')[:version]
      ['RHEL','CentOS'].each do |rel|
        process_rpm_yaml(rel,simp_version)
      end
    end

    desc 'Update the SIMP RPM list'
    task :simp do
      simp_version = Simp::RPM.get_info('build/simp-doc.spec')[:version]
      collected_data = []

      if File.directory?('../../src/build')
        default_metadata = YAML.load_file('../../src/build/package_metadata_defaults.yaml')

        Find.find('../../') do |path|
          path_basename = File.basename(path)

          # Ignore hidden directories
          unless path_basename == '..'
            Find.prune if path_basename[0].chr == '.'
          end

          # Ignore spec tests
          Find.prune if path_basename == 'spec'

          # Only Directories
          Find.prune unless File.directory?(path)
          # Ignore symlinks (this may be redundant on some systems)
          Find.prune if File.symlink?(path)

          build_dir = File.join(path,'build')
          if File.directory?(build_dir)
            Dir.chdir(path) do
              # Update the metadata for this RPM
              rpm_metadata = default_metadata.dup
              if File.exist?('build/package_metadata.yaml')
                rpm_metadata.merge!(YAML.load_file('build/package_metadata.yaml'))
              end

              valid_rpm = false
              Array(rpm_metadata['valid_versions']).each do |version_regex|
                if Regexp.new("^#{version_regex}$").match(simp_version)
                  valid_rpm = true
                  break
                end
              end

              if valid_rpm
                # Use the real RPMs if we have them
                rpms = Dir.glob('dist/*.rpm')
                rpms.delete_if{|x| x =~ /\.src\.rpm$/}
                if rpms.empty?
                  Dir.glob('build/*.spec').each do |rpm_spec|
                    pkginfo = Simp::RPM.get_info(rpm_spec)
                    pkginfo[:metadata] = rpm_metadata
                    collected_data << pkginfo
                  end
                else
                  rpms.each do |rpm|
                    pkginfo = Simp::RPM.get_info(rpm)
                    pkginfo[:metadata] = rpm_metadata
                    collected_data << pkginfo
                  end
                end
              end
            end
          end
        end
      end

      header = <<-EOM
SIMP #{simp_version} RPMs
-------------------------

This provides a comprehensive list of all SIMP RPMs and related metadata. Most
importantly, it provides a list of which modules are installed by default and
which are simply available in the repository.

      EOM

      table = [ header ]
      table << ".. list-table:: SIMP #{simp_version} RPMs"
      table << '   :widths: 30 30 30'
      table << '   :header-rows: 1'
      table << ''
      table << '   * - Name'
      table << '     - Version'
      table << '     - Optional'

      data = ['   * - Unknown']
      data << ['     - Unknown']
      data << ['     - Unknown']
      unless collected_data.empty?
        data = collected_data.sort_by{|x| x[:name]}.map{|x| x = "   * - #{x[:name]}\n     - #{x[:full_version]}\n     - #{!x[:metadata]['optional']}"}
      end

      table += data

      fh = File.open(File.join('docs','user_guide','RPM_Lists',"Core_SIMP_#{simp_version}_RPM_List.rst"),'w')
      # Highlight those items that are always there
      fh.puts(table.join("\n").gsub(',true',',**true**'))
      fh.sync
      fh.close
    end
  end

  desc 'basic linting tasks'
  task :lint do
    file_issues = Hash.new()

    puts "Starting doc linting"
    puts "Checking for bad tags..."

    file_issues.merge!(lint_files_with_bad_tags)

    unless file_issues.empty?
      msg = ["The following issues were found:"]

      file_issues.keys.each do |file|
        msg << "  * #{file}"
        msg << "    * Issue Type: #{file_issues[file][:issue_type]}"
        msg << "    * Issue Message: #{file_issues[file][:issue_text]}"
        msg << ''
      end

      $stderr.puts(msg)
      exit(1)
    end

    puts "Linting Complete"
  end

  desc 'build HTML docs'
  task :html => [:lint] do
    extra_args = ''
    ### TODO: decide how we want this task to work
    ### version = File.open('build/simp-doc.spec','r').readlines.select{|x| x =~ /^%define simp_major_version/}.first.chomp.split(' ').last
    ### extra_args = "-t simp_#{version}" if version
    cmd = "sphinx-build -E -n #{extra_args} -b html -d sphinx_cache docs html"
    run_build_cmd(cmd)
  end

  desc 'build HTML docs (single page)'
  task :singlehtml => [:lint] do
    extra_args = ''
    cmd = "sphinx-build -E -n #{extra_args} -b singlehtml -d sphinx_cache docs html-single"
    run_build_cmd(cmd)
  end

  desc 'build Sphinx PDF docs using the RTD resources (SLOWEST) TODO: BROKEN'
  task :sphinxpdf => [:lint] do
    [ "sphinx-build -E -n -b latex -D language=en -d sphinx_cache docs latex",
      "pdflatex -interaction=nonstopmode -halt-on-error ./latex/*.tex"
    ].each do |cmd|
      run_build_cmd(cmd)
    end
  end

  desc 'build PDF docs (SLOWEST)'
  task :pdf => [:lint] do
    extra_args = ''
    cmd = "sphinx-build -E -n #{extra_args} -b pdf -d sphinx_cache docs pdf"
    run_build_cmd(cmd)
  end

  desc 'Check for broken external links'
  task :linkcheck => [:lint] do
    cmd = "sphinx-build -E -n -b linkcheck -d sphinx_cache docs linkcheck"
    run_build_cmd(cmd)
  end

  desc 'run a local web server to view HTML docs on http://localhost:5000'
  task :server, [:port] => [:html] do |_t, args|
    port = args.to_hash.fetch(:port, 5000)
    puts "running web server on http://localhost:#{port}"
    %x(ruby -run -e httpd html/ -p #{port})
  end
end

# We want to prep for build if possible, but not when running `rake -T`, etc...
Rake.application.tasks.select{|task| task.name.start_with?('docs:', 'pkg:')}.each do |task|
  task.enhance ['munge:prep']
end
