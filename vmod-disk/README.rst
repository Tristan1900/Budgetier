============
vmod-disk
============

DESCRIPTION
===========

A vmod that has a threshold tuner, a probability tuner, and a static probability.

START YOUR OWN VMOD
===================

The basic steps to start a new vmod from this disk are::

  name=myvmod
  git clone libvmod-disk libvmod-$name
  cd libvmod-$name
  ./rename-vmod-script $name

and follow the instructions output by rename-vmod-script

remember to git checkout to your varnish version, different version will not compile according to my experience.
