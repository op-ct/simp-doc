.. _faq-puppet:

Puppet-Related Issues
=====================

.. _faq-puppet-debug_mode_crash:

Why is my Puppet Agent crashing when run with ``--debug``?
----------------------------------------------------------

The `FACT-1732`_ bug (present in some versions of `Facter 3`_) can cause
Facter to crash when attempting to print `Bignum`_-sized numbers (On
a 64-bit system, a ``Bignum`` value is ``(2**62)`` or higher).

This will affect runs of ``puppet agent -t --debug`` as well as ``facter -p``.

.. NOTE::

  SIMP modules' facts have not been affected by this issue since SIMP 6.1.0-0
  (before that, the ``shmall`` and ``shmax`` facts from ``simp-simplib``
  would crash on large systems).

  However, facts introduced from non-SIMP sources may still be susceptible.

.. _Bignum: https://ruby-doc.org/core-2.3.0/Bignum.html
.. _FACT-1732: https://tickets.puppetlabs.com/browse/FACT-1732
.. _Facter 3: https://docs.puppet.com/facter/3.8/

.. _faq-puppet-generate_types:

When should I run `puppet generate types`?
------------------------------------------

The ``puppet generate types`` command was added to help solve the problem of
`Puppet Environment isolation`_ (`SERVER-94`_) by generating :term:`custom type`
metadata definitions for each environment.  However, this command must be
re-run under a variety of circumstances.

Situations ``incron`` handles automatically
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

By default, SIMP configures the ``incron`` daemon to automatically run ``puppet
generate types`` under either of the following circumstances:

  * The ``puppet`` or ``puppetserver`` binaries have been updated.
  * A new :term:`Puppet environment` directory is added to the system.

This behavior is managed by the Puppet class ``pupmod::master::generate_types``.

.. NOTE::
   Earlier versions of `simp-pupmod` (7.6.0 through 7.7.1) attempted to
   automatically trigger ``puppet generate types`` under all relevant
   circumstances.  However, some triggers could potentially add too much load
   to the system, and  were removed from ``incron``'s watchlist.  These
   situations must be addressed by other means (see below).


Situations ``incron`` doesn't handle
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

``incron`` does not handle all cases, so you will need to ensure that ``puppet
generate types`` is run under the following circumstances:

  * A new *module* that includes custom types is added to an existing environment.
  * An existing custom type's internal code is updated.


Generating types manually
^^^^^^^^^^^^^^^^^^^^^^^^^

You can run the ``puppet generate types`` command as ``root`` on the Puppet
Server.  However, in order to ensure that the Puppet Server process can read
the generated files, you will have to ensure they have the correct ownership
and permissions.  One way to do this is by running the following command:

.. code-block::
  (umask 0027 && sg puppet -c 'puppet generate types --environment ENVIRONMENT')


Automatically generating types after ``r10k deploy environment``
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

If you are using :term:`r10k` to deploy branches from a :term:`Control
Repository` using ``r10k deploy environment [OPTIONS], you can set the
"`generate_types`_" option in your ``r10k.yaml`` to automatically run
``puppet generate types`` for the environment(s) deployed after ``r10k deploy
environment``:

.. code-block:: yaml

   ---
   # ...
   deploy:
     generate_types: true

.. _SERVER-94: https://tickets.puppetlabs.com/browse/SERVER-94
.. _postrun: https://github.com/puppetlabs/r10k/blob/master/doc/dynamic-environments/configuration.mkd#postrun
.. _generate_types: https://github.com/puppetlabs/r10k/blob/master/doc/dynamic-environments/configuration.mkd#generate_types
.. _Puppet Environment isolation: https://puppet.com/docs/puppet/5.5/environment_isolation.html
