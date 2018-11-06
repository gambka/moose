[GlobalParams]
#  volumetric_locking_correction = true
  displacements = 'disp_x disp_y disp_z'
[]

[Mesh]
  file = xz_plane_mesh.e
[]

[Variables]
  [./disp_x]
  [../]
  [./disp_z]
  [../]
[]

[AuxVariables]
  [./disp_y]
  [../]
  [./penetration]
  [../]
  [./diag_saved_x]
  [../]
  [./diag_saved_z]
  [../]
  [./inc_slip_x]
  [../]
  [./inc_slip_z]
  [../]
  [./accum_slip_x]
  [../]
  [./accum_slip_z]
  [../]
[]

[Modules/TensorMechanics/Master]
  [./mechanics]
    block = '1 2'
    strain = SMALL
    incremental = true
    planar_formulation = PLANE_STRAIN
    out_of_plane_direction = y
    generate_output = 'stress_xx stress_xz stress_yy stress_zz'
  [../]
[]

[AuxKernels]
  [./zeroslip_x]
    type = ConstantAux
    variable = inc_slip_x
    boundary = 4
    execute_on = timestep_begin
    value = 0.0
  [../]
  [./zeroslip_z]
    type = ConstantAux
    variable = inc_slip_z
    boundary = 4
    execute_on = timestep_begin
    value = 0.0
  [../]
  [./accum_slip_x]
    type = AccumulateAux
    variable = accum_slip_x
    accumulate_from_variable = inc_slip_x
    execute_on = timestep_end
  [../]
  [./accum_slip_z]
    type = AccumulateAux
    variable = accum_slip_z
    accumulate_from_variable = inc_slip_z
    execute_on = timestep_end
  [../]
  [./penetration]
    type = PenetrationAux
    variable = penetration
    boundary = 4
    paired_boundary = 3
  [../]
[]

[Postprocessors]
  # [./bot_react_x]
  #   type = NodalSum
  #   variable = saved_x
  #   boundary = 1
  # [../]
  # [./bot_react_z]
  #   type = NodalSum
  #   variable = saved_z
  #   boundary = 1
  # [../]
  # [./top_react_x]
  #   type = NodalSum
  #   variable = saved_x
  #   boundary = 5
  # [../]
  # [./top_react_z]
  #   type = NodalSum
  #   variable = saved_z
  #   boundary = 5
  # [../]
  # [./ref_resid_x]
  #   type = NodalL2Norm
  #   execute_on = timestep_end
  #   variable = saved_x
  # [../]
  # [./ref_resid_z]
  #   type = NodalL2Norm
  #   execute_on = timestep_end
  #   variable = saved_z
  # [../]
  [./sigma_yy]
    type = ElementAverageValue
    variable = stress_yy
  [../]
  [./sigma_zz]
    type = ElementAverageValue
    variable = stress_zz
  [../]
  [./disp_x2]
    type = NodalVariableValue
    nodeid = 1
    variable = disp_x
  [../]
  [./disp_x7]
    type = NodalVariableValue
    nodeid = 6
    variable = disp_x
  [../]
  [./disp_z2]
    type = NodalVariableValue
    nodeid = 1
    variable = disp_z
  [../]
  [./disp_z7]
    type = NodalVariableValue
    nodeid = 6
    variable = disp_z
  [../]
  [./_dt]
    type = TimestepSize
  [../]
  [./num_lin_it]
    type = NumLinearIterations
  [../]
  [./num_nonlin_it]
    type = NumNonlinearIterations
  [../]
[]

[BCs]
  [./bot_z]
    type = DirichletBC
    variable = disp_z
    boundary = '1'
    value = 0.0
  [../]
  [./side_x]
    type = DirichletBC
    variable = disp_x
    boundary = 2
    value = 0.0
  [../]
  [./top_press]
    type = Pressure
    variable = disp_z
    boundary = 5
    component = 2
    factor = 109.89
  #  factor = 1
  [../]
[]

[Materials]
  [./bot_elas_tens]
    type = ComputeIsotropicElasticityTensor
    block = '1'
    youngs_modulus = 1e6
    poissons_ratio = 0.3
  [../]
  [./bot_stress]
    type = ComputeFiniteStrainElasticStress
    block = '1'
  [../]
  [./top_elas_tens]
    type = ComputeIsotropicElasticityTensor
    block = '2'
    youngs_modulus = 1e6
    poissons_ratio = 0.3
  [../]
  [./top_stress]
    type = ComputeFiniteStrainElasticStress
    block = '2'
  [../]
[]

[Executioner]
  type = Transient
  solve_type = 'PJFNK'

  petsc_options_iname = '-pc_type -pc_factor_mat_solver_package'
  petsc_options_value = 'lu     superlu_dist'

  line_search = 'none'

  nl_abs_tol = 1e-10
  nl_rel_tol = 1e-9
  l_max_its = 50
  nl_max_its = 100
  dt = 1.0
  end_time = 1.0
  num_steps = 10
  dtmin = 1.0
  l_tol = 1e-3
[]

[VectorPostprocessors]
  [./x_disp]
    type = NodalValueSampler
    variable = disp_x
    boundary = '1 3 4 5'
    sort_by = x
  [../]
  [./cont_press]
    type = NodalValueSampler
    variable = contact_pressure
    boundary = '3'
    sort_by = x
  [../]
[]

[Outputs]
  print_linear_residuals = true
  perf_graph = true
  execute_on = 'INITIAL NONLINEAR TIMESTEP_END'
  [./out]
    type = Exodus
    output_dimension = 3
  [../]
  [./console]
    type = Console
    max_rows = 5
  [../]
  [./chkfile]
    type = CSV
    show = 'disp_x2 disp_z2 disp_x7 disp_z7 sigma_yy sigma_zz x_disp cont_press'
    execute_vector_postprocessors_on = timestep_end
  [../]
  [./outfile]
    type = CSV
    delimiter = ' '
    execute_vector_postprocessors_on = none
  [../]
[]

[Contact]
  [./leftright]
    slave = 3
    master = 4
    model = frictionless
    formulation = kinematic
    out_of_plane_direction = y
    displacements = 'disp_x disp_z'
    system = constraint
    normalize_penalty = true
    tangential_tolerance = 1e-3
    penalty = 1e+9
  [../]
[]
