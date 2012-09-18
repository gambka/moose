# This input file is designed to test the LinearElasticMaterial class. 

[Mesh]
  type = GeneratedMesh
  dim = 2
  nx = 10
  ny = 10
  nz = 0
  xmin = 0
  xmax = 50
  ymin = 0
  ymax = 50
  zmin = 0
  zmax = 0
  elem_type = QUAD4
[]

[Variables]
  [./disp_x]
    order = FIRST
    family = LAGRANGE
  [../]
  [./disp_y]
    order = FIRST
    family = LAGRANGE
  [../]
[]

[AuxVariables]
  # stresses
  [./s11_aux]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./s12_aux]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./s13_aux]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./s22_aux]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./s23_aux]
    order = CONSTANT
    family = MONOMIAL
  [../]
  [./s33_aux]
    order = CONSTANT
    family = MONOMIAL
  [../]
[]

[TensorMechanics]
  [./solid]
    disp_x = disp_x
    disp_y = disp_y
    applied_strain_vector = '0.0014 -0.005 0 0 0 0'
  [../]
[]

[AuxKernels]
  [./matl_s11]
    type = RankTwoAux
    rank_two_tensor = stress
    index_i = 1
    index_j = 1
    variable = s11_aux
  [../]
  [./matl_s12]
    type = RankTwoAux
    rank_two_tensor = stress
    index_i = 1
    index_j = 2
    variable = s12_aux
  [../]
  [./matl_s13]
    type = RankTwoAux
    rank_two_tensor = stress
    index_i = 1
    index_j = 3
    variable = s13_aux
  [../]
  [./matl_s22]
    type = RankTwoAux
    rank_two_tensor = stress
    index_i = 2
    index_j = 2
    variable = s22_aux
  [../]
  [./matl_s23]
    type = RankTwoAux
    rank_two_tensor = stress
    index_i = 2
    index_j = 3
    variable = s23_aux
  [../]
  [./matl_s33]
    type = RankTwoAux
    rank_two_tensor = stress
    index_i = 3
    index_j = 3
    variable = s33_aux
  [../]
[]

[Materials]
  [./Anisotropic]
    # set from elk/tests/anisotropic_path/anisotropic_patch_test.i 
    # reading C_11  C_12  C_13  C_22  C_23  C_33  C_44  C_55  C_66
    type = LinearElasticMaterial
    block = 0
    disp_x = disp_x
    disp_y = disp_y
    all_21 = false
    C_ijkl = '1.0e6  0.0   0.0 1.0e6  0.0  1.0e6 0.5e6 0.5e6 0.5e6'
  [../]
[]

[BCs]
  active = 'Periodic'
  [./Periodic]
    active = ''
    [./all]
      auto_direction = 'x y'
    [../]
  [../]
[]

[Executioner]
  type = Steady
  petsc_options = '-snes_mf_operator'
[]

[Output]
  file_base = Applied
  output_initial = true
  exodus = true
  perf_log = true
[]

