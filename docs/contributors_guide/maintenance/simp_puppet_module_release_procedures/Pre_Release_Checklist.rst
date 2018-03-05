Puppet Module Pre-Release Checklist
===================================

The bulk of the work to release a component is to verify that the
component is ready for release.  Below is the list of verifications
that must be executed before proceeding with the release.  If any
of these checks fail, the problem identified must be fixed before
you can proceed with the tag and release steps.

* `Verify a release is warranted`_
* `Verify the CHANGELOG`_
* `Verify the component's dependencies`_
* `Verify a Puppet module can be created`_
* `Verify RPMs can be created`_
* `Verify unit tests pass`_
* `Verify acceptance tests pass`_
* `Tag and Release`_
* `Additional steps to release RPMs`_

Verify a release is warranted
-----------------------------

This check verifies a new release is warranted and the version has been
properly update:

#. Clone the component repository and checkout the development
   branch to be tagged

   .. code-block:: bash

      git clone https://github.com/simp/pupmod-simp-iptables.git
      cd pupmod-simp-iptables
      git checkout master # this step isn't needed for master branch

#. Run the ``compare_latest_tag`` rake task

   .. code-block:: bash

      bundle update
      bundle exec rake compare_latest_tag

   .. IMPORTANT::

      If this check indicates no new tag is required, there
      is no reason to continue with the release procedures.

Verify the CHANGELOG
--------------------

This check verifies that the CHANGELOG information can be properly
extracted:

#. Run the ``changelog_annotation`` rake task

   .. code-block:: bash

      bundle exec rake changelog_annotation

#. Manually verify the changelog information is emitted.

   * It should begin with ``Release of x.y.z`` and then be followed by
     one or more comment blocks. For example,

     .. code-block:: none

      Release of 6.0.3

      * Thu Aug 10 2017 Nick Markowski <nmarkowski@keywcorp.com> - 6.0.3-0
        - Updated iptables::listen::tcp_stateful example to pass valid
          Iptables::DestPort types to dports

   * It should be understandable.
   * It should be free from typos.
   * Any parsing error messages emitted should *only* be for changelog
     entries for earlier versions.

.. IMPORTANT::

   The changelog information emitted will be used as the content
   of the `GitHub`_ release notes.

Verify the component's dependencies
-----------------------------------

This check verifies the component's dependencies are correct in the
``metadata.json``:

* Verify that the dependencies in the ``metadata.json`` file
  are complete.  This means that the sources of all external
  functions/classes used within the module are  listed in
  the ``metadata.json``.

* Verify that the version constraints for each dependency are
  correct.

.. IMPORTANT::

   Beginning with ``simp-rake-helpers-4.1.0``, the RPM dependencies
   for a component will determined from its ``metadata.json`` file,
   and if present, the component's entry in the
   ``simp-core/build/rpm/dependencies.yaml``.

Verify a Puppet module can be created
-------------------------------------

This check verifies that a `PuppetForge`_-deployable Puppet module can
be created:

.. code-block:: bash

   bundle exec rake metadata_lint
   puppet module build

Verify RPMs can be created
--------------------------

This check verifies that an RPM can be generated for this module from
``simp-core``:

#. Clone ``simp-core``

   .. code-block:: bash

      git clone https://github.com/simp/simp-core.git

#. Update the URL for the component under test ``Puppetfile.tracking``,
   if needed

   .. code-block:: bash

      cd simp-core
      vi Puppetfile.tracking

#.  Build RPM

   .. code-block:: bash

      bundle update
      bundle exec rake deps:checkout
      bundle exec rake pkg:single[iptables]

.. NOTE::

   This command will build the RPM for the OS of the server
   on which it was executed.

Verify unit tests pass
----------------------

This check verifies that the component's unit tests have succeeded
in `TravisCI`_:

* Navigate to the project's TravisCI results page and verify the
  tests for the development branch to be tagged and released have
  passed.  For our project, this page is
  https://travis-ci.org/simp/pupmod-simp-iptables/branches

.. IMPORTANT::

   If the tests in TravisCI fail, you **must** fix them before
   proceeding.  The automated release procedures will only
   succeed, if the unit tests succeed in TravisCI.

Verify acceptance tests pass
----------------------------

This check verifies that the component's acceptance tests have
succeeded:

* Run the ``beaker:suites`` rake task with and without FIPS enabled

  .. code-block:: bash

     BEAKER_fips=yes bundle exec rake beaker:suites
     bundle exec rake beaker:suites

.. NOTE::

   * For older projects that have not been updated to use test
     suites, you may have to run the ``acceptance`` rake task,
     instead.

   * If the GitLab instance for the project is current (it is
     sync'd every 3 hours), you can look at the latest acceptance
     test results run by GitLab.  For our project, the results will
     be at https://gitlab.com/simp/pupmod-simp-iptables/pipelines.



Tag and Release
---------------

After the verifications above have been completed, the Puppet module is ready
for a tag and release using the `Release to GitHub and Deploy to PuppetForge_`
procedure.


Additional steps to release RPMs
--------------------------------

RPM releases require additional integration testing, documented


.. _GitHub: https://github.com
.. _PuppetForge: https://forge.puppet.com
.. _TravisCI: https://travis-ci.org
