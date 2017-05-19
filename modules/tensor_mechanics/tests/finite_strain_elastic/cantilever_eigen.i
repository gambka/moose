#
# Cantiliver Beam Test
#

[Mesh]
  type = GeneratedMesh
  dim = 3
  elem_type = HEX8
  nx = 10
  ny = 5
  nz = 5
  xmin = 0
  xmax = 2
  ymin = 0
  ymax = 0.5
  zmin = 0
  zmax = 0.5
[]

[GlobalParams]
  displacements = 'disp_x disp_y disp_z'
[]

[Functions]
  [./displacement_ramp]
    type = ParsedFunction
    value = '-1e-3 * t'
  [../]
[]

[Modules]
  [./TensorMechanics]
    [./Master]
      [./all]
        strain = FINITE
        add_variables = true
        decomposition_method = EigenSolution
        volumetric_locking_correction = false
        generate_output = 'stress_xx stress_yy stress_zz stress_xy stress_yz stress_zx'
      [../]
    [../]
  [../]
[]

[BCs]
  [./no_x]
    type = PresetBC
    variable = disp_x
    boundary = left
    value = 0.0
  [../]
  [./no_y]
    type = PresetBC
    variable = disp_y
    boundary = left
    value = 0.0
  [../]
  [./no_z]
    type = PresetBC
    variable = disp_z
    boundary = left
    value = 0.0
  [../]
  [./pull]
    type = FunctionPresetBC
    variable = disp_y
    boundary = right
    function = displacement_ramp
  [../]
[]

[Materials]
  [./stress]
    type = ComputeFiniteStrainElasticStress
  [../]
  [./elasticity_tensor]
    type = ComputeIsotropicElasticityTensor
    youngs_modulus = 1e6
    poissons_ratio = 0.3
  [../]
[]

[Preconditioning]
  [./SMP]
    type = SMP
    full = true
  [../]
[]

[Executioner]

  type = Transient

  #Preconditioned JFNK (default)
  solve_type = 'PJFNK'



  petsc_options_iname = '-pc_type -sub_pc_type'
  petsc_options_value = 'asm       lu'

  nl_rel_tol = 1e-8

  l_tol = 1e-05

  l_max_its = 50

  start_time = 0.0
  dt = 5
  end_time = 500
[] # Executioner

[Outputs]
  exodus = true
  print_perf_log = true
[]
