[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 20
  ny = 20
[]

[Variables]
  [./u]
  [../]
[]

[Functions]
  [./tif]
    type = ImageFunction
    file_base = stack/test
    file_range = 00
    flip_x = true
  [../]
[]

[ICs]
  [./u_ic]
    type = FunctionIC
    function = tif
    variable = u
  [../]
[]

[Problem]
  type = FEProblem
  solve = false
[]

[Executioner]
  # Preconditioned JFNK (default)
  type = Transient
  num_steps = 1
  dt = 0.1
[]

[Outputs]
  output_initial = true
  exodus = true
  [./console]
    type = Console
    perf_log = true
    nonlinear_residuals = false
    linear_residuals = false
  [../]
[]
