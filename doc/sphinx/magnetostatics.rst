.. _Magnetostatics:

Magnetostatics
==============

.. _Dipolar interaction:

Dipolar interaction
-------------------

|es| contains methods to calculate the interactions between point dipoles

.. math::

   U^{Dip}(\vec{r}) = D \cdot \left( \frac{(\vec{\mu}_i \cdot \vec{\mu}_j)}{r^3}
     - \frac{3  (\vec{\mu}_i \cdot \vec{r})  (\vec{\mu}_j \cdot \vec{r}) }{r^5} \right)

where :math:`r=|\vec{r}|`.
The prefactor :math:`D` is can be set by the user and is given by

.. math::
   D =\frac{\mu_0 \mu}{4\pi}
   :label: dipolar_prefactor

where :math:`\mu_0` and :math:`\mu` are the vacuum permittivity and the
relative permittivity of the background material, respectively.

Magnetostatic interactions are activated via the actor framework::

    import espressomd
    import espressomd.magnetostatics

    system = espressomd.System(box_l=[10, 10, 10])
    system.time_step = 0.01
    system.part.add(pos=[[0, 0, 0], [1, 1, 1]], dip=2 * [(1, 0, 0)],
                    rotation=2 * [(True, True, True)])

    actor = espressomd.magnetostatics.DipolarDirectSumCpu(prefactor=1.)
    system.actors.add(actor)

The list of actors can be cleared with
:meth:`system.actors.clear() <espressomd.actors.Actors.clear>` and
:meth:`system.actors.remove(actor) <espressomd.actors.Actors.remove>`.


.. _Dipolar P3M:

Dipolar P3M
~~~~~~~~~~~

:class:`espressomd.magnetostatics.DipolarP3M`

This is the dipolar version of the P3M algorithm, described in :cite:`cerda08d`.

Make sure that you know the relevance of the P3M parameters before using
P3M! If you are not sure, read the following references:
:cite:`ewald21a,hockney88a,kolafa92a,deserno98a,deserno98b,deserno00e,deserno00b,cerda08d`.

Note that dipolar P3M does not work with non-cubic boxes.


The parameters of the dipolar P3M method can be tuned automatically, by
providing ``accuracy=<TARGET_ACCURACY>`` to the method. It is also possible to
pass a subset of the method parameters such as ``mesh``. In that case, only
the omitted parameters are tuned::

    import espressomd.magnetostatics as magnetostatics
    p3m = magnetostatics.DipolarP3M(prefactor=1, mesh=32, accuracy=1E-4)
    system.actors.add(p3m)

It is important to note that the error estimates given in :cite:`cerda08d`
used in the tuning contain assumptions about the system. In particular, a
homogeneous system is assumed. If this is no longer the case during the
simulation, actual force and torque errors can be significantly larger.


.. _Dipolar Layer Correction (DLC):

Dipolar Layer Correction (DLC)
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

:class:`espressomd.magnetostatics.DLC`

The dipolar layer correction (DLC) is used in conjunction with the dipolar P3M
method to calculate dipolar interactions in a 2D-periodic system.
It is based on :cite:`brodka04a` and the dipolar version of
:ref:`Electrostatic Layer Correction (ELC)`.

Usage notes:

* The non-periodic direction is always the **z-direction**.

* The method relies on a slab of the simulation box perpendicular to the
  z-direction not to contain particles. The size in z-direction of this slab
  is controlled by the ``gap_size`` parameter. The user has to ensure that
  no particles enter this region by means of constraints or by fixing the
  particles' z-coordinate. When particles enter the slab of the specified
  size, an error will be thrown.

* The method can be tuned using the ``accuracy`` parameter. In contrast to
  the electrostatic method, it refers to the energy. Furthermore, it is
  assumed that all dipole moment are as large as the largest of the dipoles
  in the system.

* When the base solver is not a P3M method, metallic epsilon is assumed.

The method is used as follows::

    import espressomd.magnetostatics
    dp3m = espressomd.magnetostatics.DipolarP3M(prefactor=1, accuracy=1E-4)
    mdlc = espressomd.magnetostatics.DLC(actor=dp3m, maxPWerror=1E-5, gap_size=2.)
    system.actors.add(mdlc)


.. _Dipolar direct sum:

Dipolar direct sum
------------------

This interaction calculates energies and forces between dipoles by
explicitly summing over all pairs. For the directions in which the
system is periodic (as defined by ``system.periodicity``), it applies the
minimum image convention, i.e. the interaction is effectively cut off at
half a box length.

The direct summation methods are mainly intended for non-periodic systems
which cannot be solved using the dipolar P3M method.
Due to the long-range nature of dipolar interactions, direct summation with
minimum image convention does not yield good accuracy with periodic systems.

Two methods are available:

* :class:`~espressomd.magnetostatics.DipolarDirectSumCpu`
  performs the calculation in double-precision on the CPU,
  optionally with replicas.

* :class:`~espressomd.magnetostatics.DipolarDirectSumGpu`
  performs the calculations in single-precision on a CUDA-capable GPU.
  The implementation is optimized for large systems of several thousand
  particles. It makes use of one thread per particle. When there are fewer
  particles than the number of threads the GPU can execute simultaneously,
  the rest of the GPU remains idle. Hence, the method will perform poorly
  for small systems.

To use the methods, create an instance of either
:class:`~espressomd.magnetostatics.DipolarDirectSumCpu` or
:class:`~espressomd.magnetostatics.DipolarDirectSumGpu` and add it to the
system's list of active actors. The only required parameter is the prefactor
:eq:`dipolar_prefactor`::

    import espressomd.magnetostatics
    dds = espressomd.magnetostatics.DipolarDirectSumGpu(prefactor=1)
    system.actors.add(dds)

The CPU implementation has an optional argument ``n_replicas`` which
adds periodic copies to the system along periodic directions. In that
case, the minimum image convention is no longer used.

Both implementations support MPI-parallelization.


.. _Barnes-Hut octree sum on GPU:

Barnes-Hut octree sum on GPU
----------------------------

:class:`espressomd.magnetostatics.DipolarBarnesHutGpu`

This interaction calculates energies and forces between dipoles by
summing over the spatial octree cells (aka ``leaves``).
Far enough cells are considered as a single dipole with a cumulative
vector in the cell center of mass. Parameters which determine that the
cell is far enough are :math:`I_{\mathrm{tol}}^2` and
:math:`\varepsilon^2` which define a fraction of the cell and
an additive distance respectively. For the detailed description of the
Barnes-Hut method application to the dipole-dipole interactions, please
refer to :cite:`polyakov13a`.

To use the method, create an instance of :class:`~espressomd.magnetostatics.DipolarBarnesHutGpu`
and add it to the system's list of active actors::

    import espressomd.magnetostatics
    bh = espressomd.magnetostatics.DipolarBarnesHutGpu(prefactor=1., epssq=200.0, itolsq=8.0)
    system.actors.add(bh)


.. _ScaFaCoS magnetostatics:

ScaFaCoS magnetostatics
-----------------------

:class:`espressomd.magnetostatics.Scafacos`

|es| can use the methods from the ScaFaCoS *Scalable fast Coulomb solvers*
library for dipoles, if the methods support dipolar calculations. The feature
``SCAFACOS_DIPOLES`` has to be added to :file:`myconfig.hpp` to activate this
feature. Dipolar calculations are only included in the ``dipoles`` branch of
the ScaFaCoS code. The specific methods available can be queried with
:meth:`espressomd.electrostatics.Scafacos.get_available_methods`.

To use ScaFaCoS, create an instance of :class:`~espressomd.magnetostatics.Scafacos`
and add it to the list of active actors. Three parameters have to be specified:
``prefactor``, ``method_name``, ``method_params``. The method-specific
parameters are described in the ScaFaCoS manual. In addition, methods
supporting tuning have a parameter ``tolerance_field`` which sets the desired
root mean square accuracy for the magnetic field.

For details of the various methods and their parameters please refer to
the ScaFaCoS manual. To use this feature, ScaFaCoS has to be built as a
shared library.
