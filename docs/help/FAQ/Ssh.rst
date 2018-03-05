
How do I manage additional settings for SSHD?
=============================================

To manage SSHD settings the SIMP `ssh` module doesn't cover, use the
`sshd_config` resource from `augeasproviders_ssh`_.  This is what the SIMP
`ssh` module uses internally.

For instance, to set the sshd `LogLevel`_ option to ``VERBOSE``:

.. code-block:: puppet

   # VERBOSE will log SSH key fingerprints used for logins
   sshd_config { 'LogLevel' : value => 'VERBOSE' }

https://github.com/simp/pupmod-simp-ssh/blob/master/manifests/server/conf.pp#L244


How do I manage additional settings for SSH?
============================================

The simp
In the meanwhile, users can customize their
ssh_config configuration by editing $HOME/.ssh/config.


.. _augeasproviders_ssh: http://augeasproviders.com/documentation/examples.html#sshdconfig-provider
.. _augeasproviders_ssh_gh: https://github.com/hercules-team/augeasproviders_ssh#sshd_config-provider
.. _LogLevel: https://www.ssh.com/ssh/sshd_config/#sec-Verbose-logging
