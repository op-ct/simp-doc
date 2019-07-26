.. _ug-sa-env-deployment-scenarios:

Deployment Scenarios
--------------------

.. include:: <isonum.txt>
.. include:: <isopub.txt>

SIMP is developed to support deploying modules and environments using r10k
under the following scenarios:

.. list-table:: Deployment Scenarios
   :header-rows: 1
   :stub-columns: 1
   :align: left

   * - Deployment scenario
     - Puppet Module Repos
     - Control Repository?
     - r10k command
     - Notes
   * - :ref:`Local <ug-sa-env-deployment-scenarios--local>`
     - Local :ref:`(from RPMs) <ug-sa-env-local-module-repositories>`
     - |cross| No
     - ``puppetfile install``
     -
   * - :ref:`Control Repository <ug-sa-env-deployment-scenarios--controlrepo>`
     - Remote
     - |check| Yes
     - ``deploy environment``
     -
   * - :ref:`Webhook + Control Repository <ug-sa-env-deployment-scenarios--webhook+controlrepo>`
     - Remote
     - |check| Yes
     - ``deploy environment``
     - |copy| 2000


.. |r10k_local_repos__no_control_repo| image:: ../../images/diagrams/deployment_scenarios/r10k_local_repos__no_control_repo.png
    :alt: r10k + local, SIMP RPM-delivered module repositories

.. |r10k__control_repos| image:: ../../images/diagrams/deployment_scenarios/r10k__control_repos.png
    :alt: r10k + Control Repository

.. |enterprise__automated_deploy| image:: ../../images/diagrams/deployment_scenarios/enterprise__automated_deploy.png
    :alt: PE Code Manager + Control Repository

.. _ug-sa-env-deployment-scenarios--local:

Local: ``r10k puppetfile`` + SIMP RPM-delivered module repositories
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

This is the initial deployment scenario immediately after
:ref:`gsg-installing_simp_from_an_iso`.

|r10k_local_repos__no_control_repo|

Under this scenario, *local* Puppet admins:

* Deploy Puppet modules from an existing Puppet environment directory
* Use the command ``r10k puppetfile install`` to deploy the modules defined in
  the local ``Puppetfile``
* Reference module repos from Git repositories on the local filesystem
  via ``file://`` urls in the ``Puppetfile``.

.. _ug-sa-env-deployment-scenarios--controlrepo:


Control Repository Deployment scenarios
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^


Scenario 1
~~~~~~~~~~

|r10k__control_repos|




Under this scenario, Puppet admins:

* Maintain a Puppet control repository from an external git service
* Manually Deploy entire :term:`Puppet environment` directories (one per
  branch):
  * ``r10k deploy environment [ENVIRONMENT]`` (r10k)
  * ``puppet code deploy [ENVIRONMENT`` (Puppet Enterprise)
* This might be done manually from the command line, or triggered
  automatically (e.g., from a webhook).

.. NOTE::

   Administrators of all control repository scenarios are assumed to have enough
   git and r10k (or Code Manager) expertise to manage their own infrastructure.
   This includes:

     * Managing a :term:`Control Repository`
     * Maintaining repositories in an external git-hosting service
     * Keeping remote Puppet module repositories up-to-date with the
       versions that SIMP RPMs deliver to the local filesystem.

.. _ug-sa-env-deployment-scenarios--webhook+controlrepo:

Webhook deploying from Code Repository
^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

|enterprise__automated_deploy|

Under this scenario, Puppet admins:


* Maintain a Puppet control repository from an external git service
* Deploy entire Puppet environment directories (one environment for each
  repository branch) using PE Code Manager.

  * This is triggered automatically from a Code Manager webhook.
