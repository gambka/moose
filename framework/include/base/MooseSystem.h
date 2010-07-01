#ifndef MOOSESYSTEM_H
#define MOOSESYSTEM_H

#include <vector>
#include <list>

//MOOSE includes
#include "Moose.h"  // for THREAD_ID
#include "KernelHolder.h"
#include "BCHolder.h"
#include "AuxHolder.h"
#include "MaterialHolder.h"
#include "StabilizerHolder.h"
#include "ICHolder.h"
#include "ElementData.h"
#include "FaceData.h"
#include "AuxData.h"
#include "MaterialData.h"

//libmesh includes
#include "transient_system.h"
#include "dof_map.h"
#include "mesh_base.h"

//Forward Declarations
class EquationSystems;
class MeshBase;
class Material;
template<class T> class NumericVector;

/**
 * This class represents one full coupled system of nonlinear equations including any
 * explicit (Aux) equations.
 *
 * You can have multiple MOOSE systems... and (one day) couple them together).
 */
class MooseSystem
{
public:
  MooseSystem();
  MooseSystem(Mesh &mesh);
  virtual ~MooseSystem();

  /**
   * Data Accessors for the various FE datastructures indexed by thread
   */
  //ElementData * getElementData(THREAD_ID tid);
  //FaceData * getFaceData(THREAD_ID tid);
  //AuxData * getAuxData(THREAD_ID tid);

  /**
   * Initialize the system
   */
  void init();

  /**
   * Initialize the Mesh for this MooseSystem and return a reference
   */
  Mesh * initMesh(unsigned int dim);

  /**
   * Returns a writable reference to the mesh held wihin this MooseSystem
   */
  Mesh * getMesh(bool skip_full_check=false);

  inline unsigned int getDim() { return _dim; }
  
  /**
   * Initialize the EquationSystems object and add both the nonlinear and auxiliary systems
   * to that object for this MooseSystem
   */
  EquationSystems * initEquationSystems();

  /**
   * Returns a writable reference to the EquationSystems object held within this MooseSystem
   */
  EquationSystems * getEquationSystems();

  /**
   * Returns a reference to the main nonlinear system in this instance of MooseSystem
   */
  TransientNonlinearImplicitSystem * getNonlinearSystem();
  
  /**
   * Returns a reference to the auxillary system in this instance of MooseSystem
   */
  TransientExplicitSystem * getAuxSystem();
  
  /**
   * Get the reference either to _element_data or _face_data from MooseSystem
   */
  QuadraturePointData &getQuadraturePointData(bool is_boundary);

  /**
   * Checks if we have a variable named 'var_name
   */
  bool hasVariable(const std::string &var_name);

  /**
   * Checks if we have an auxiliary variable named 'var_name
   */
  bool hasAuxVariable(const std::string &var_name);

  unsigned int getVariableNumber(const std::string &var_name);

  unsigned int getAuxVariableNumber(const std::string &var_name);

  /**
   * Computes the modified variable number for an auxiliary variable.
   * This is the variable number that Kernels know this variable to operate under.
   *
   * This is necessary because Kernels need unique variable numbers for computing
   * off-diagonal jacobian components.
   */
  unsigned int modifiedAuxVarNum(unsigned int var_num);

  /**
   * Initialize all of the FE datastructures
   */
  void initDataStructures();

  /**
   * Check to see if MooseSystem is in a workable state before accessing data
   */
  void checkValid();

  /**
   * Get the Exodus Reader for this system.
   */
  ExodusII_IO * getExodusReader();

  unsigned int addVariable(const std::string &var, const FEType  &type, const std::set< subdomain_id_type  > *const active_subdomains=NULL);
  unsigned int addVariable(const std::string &var, const Order order=FIRST, const FEFamily=LAGRANGE, const std::set< subdomain_id_type > *const active_subdomains=NULL);

  void addKernel(std::string kernel_name,
                 std::string name,
                 InputParameters parameters);

  void addBC(std::string bc_name,
             std::string name,
             InputParameters parameters);

  void addAuxKernel(std::string aux_name,
                    std::string name,
                    InputParameters parameters);

  // FIXME: was AuxKernel::addBC
  void addAuxBC(std::string aux_name,
                std::string name,
                InputParameters parameters);

  void addMaterial(std::string mat_name,
                   std::string name,
                   InputParameters parameters);

  void addStabilizer(std::string stabilizer_name,
                     std::string name,
                     InputParameters parameters);

  // FIXME: var_name should go to 'parameters'?
  void addInitialCondition(std::string ic_name,
                           std::string name,
                           InputParameters parameters,
                           std::string var_name);

  /**
   * Computes a block diagonal jacobian for the full system.
   */
  void compute_jacobian (const NumericVector<Number>& soln, SparseMatrix<Number>&  jacobian);

  /**
   * Computes one block of the jacobian.
   *
   * @param ivar The block row to compute.
   * @param jvar The block column to compute.
   */
  void compute_jacobian_block (const NumericVector<Number>& soln, SparseMatrix<Number>&  jacobian, System& precond_system, unsigned int ivar, unsigned int jvar);

  void compute_residual (const NumericVector<Number>& soln, NumericVector<Number>& residual);

  Number initial_value (const Point& p, const Parameters& parameters, const std::string& sys_name, const std::string& var_name);

  Gradient initial_gradient (const Point& p, const Parameters& parameters, const std::string& sys_name, const std::string& var_name);

  void initial_condition(EquationSystems& es, const std::string& system_name);

  void reinitKernels(THREAD_ID tid, const NumericVector<Number>& soln, const Elem * elem, DenseVector<Number> * Re, DenseMatrix<Number> * Ke = NULL);

  void reinitBCs(THREAD_ID tid, const NumericVector<Number>& soln, const Elem * elem, const unsigned int side, const unsigned int boundary_id);
  void reinitBCs(THREAD_ID tid, const NumericVector<Number>& soln, const Node & node, const unsigned int boundary_id, NumericVector<Number>& residual);

  void reinitAuxKernels(THREAD_ID tid, const NumericVector<Number>& soln, const Node & node);
  void reinitAuxKernels(THREAD_ID tid, const NumericVector<Number>& soln, const Elem & elem);

  void subdomainSetup(THREAD_ID tid, unsigned int block_id);
  /**
   * Re-Initializes temporal discretization/transient control data.
   */
  void reinitDT();

  /**
   * Copy the old solutions backwards
   */
  void copy_old_solutions();

  /**
   * Re-Initializes Eigenvalue computation
   */
  void reinitEigen();

  void checkSystemsIntegrity();

  /**
   * Allows specification of per variable scaling factors.
   * The size of the vector MUST be the same as the number of Nonlinear Variables.
   * Should be called after Kernel::init() (because that sets the default scaling).
   * Can be called multiple times to change the scaling.
   * The initial scaling is just 1 for each variable.
   */
  void setVarScaling(std::vector<Real> scaling);

  /**
   * Get access to the active_local_element_range
   * Automatically builds it if it hasn't been initialized.
   */
  ConstElemRange * getActiveLocalElementRange();

  /**
   * Should be called after the mesh has been modified in any way.
   */
  void meshChanged();

  virtual void solve();

  /**
   * Get the EquationSystems params
   */
  Parameters &parameters();

  /**
   * Output the given system to output files.
   */
  void output_system(unsigned int t_step, Real time);


  void setPreconditioner(Preconditioner<Real> *pc) { _preconditioner = pc; }

  bool & dontReinitFE();

  Material * getMaterial(THREAD_ID tid, unsigned int block_id);
  
protected:
  void sizeEverything();

  void update_aux_vars(const NumericVector<Number>& soln);

private:
  // TODO: Switch these to vectors later
  ElementData _element_data;
  FaceData _face_data;
  AuxData _aux_data;
  MaterialData _material_data;
  //  std::vector<ElementData *> _element_data;
  //  std::vector<FaceData *> _face_data;
  //  std::vector<AuxData *> _aux_data;

  DofMap * _dof_map;

  std::vector<std::vector<unsigned int> > _dof_indices;

  /**
   * Dof Maps for all the variables.
   */
  std::vector<std::vector<std::vector<unsigned int> > > _var_dof_indices;

  DofMap * _aux_dof_map;

  /**
   * Holds the current dof numbers for each variable
   */
  std::vector<std::vector<unsigned int> > _aux_var_dofs;

  /**
   * Dof Maps for all the auxiliary variables.
   */
  std::vector<std::vector<std::vector<unsigned int> > > _aux_var_dof_indices;

  EquationSystems * _es;
  TransientNonlinearImplicitSystem * _system;
  TransientExplicitSystem * _aux_system;
  Mesh * _mesh;
  bool _delete_mesh;                            // true if we own the mesh and we are responsible for its destruction
  unsigned int _dim;

  /**
   * Whether or not the mesh has changed recently.  Useful for doing separate output.
   */
  bool _mesh_changed;

  KernelHolder _kernels;
  BCHolder _bcs;
  AuxHolder _auxs;
  MaterialHolder _materials;
  StabilizerHolder _stabilizers;
  ICHolder _ics;

  /**
   * Whether or not we need to recompute the shape functions for each element.  Should only be true if EVERY element is exactly
   * the same shape.
   */
  bool _no_fe_reinit;

  /**
   * Preconditioner
   */
  Preconditioner<Real> * _preconditioner;

  /**
   * The ExodusIO Reader to support reading of solutions at element qps
   */
  ExodusII_IO * _exreader;

  bool _is_valid;

public:
  /**
   * Current time.
   */
  Real _t;

  /**
   * Current dt.
   */
  Real _dt;

  /**
   * Old dt.
   */
  Real _dt_old;

  /**
   * Whether or not the current simulation is transient.
   */
  bool _is_transient;

  /**
   * Whether or not the current simulation is Eigenvalue.
   */
  bool _is_eigenvalue;

  /**
   * Current time step.
   */
  int _t_step;

  /**
   * Coefficients (weights) for the BDF2 time discretization.
   */
  std::vector<Real> _bdf2_wei;

  /**
   * Time discretization scheme: 0 - Implicit Euler, 1 - 2nd-order Backward Difference
   */
  short _t_scheme;

  /**
   * The total number of Runge-Kutta stages
   */
  short _n_of_rk_stages;
  
  /**
   * Maximum quadrature order required by all variables.
   */
  Order _max_quadrature_order;

  /**
   * Scaling factors for each variable.
   */
  std::vector<Real> _scaling_factor;

  /**
   * Convenience zeros.
   */

public:
  MooseArray<Real> _real_zero;
  MooseArray<MooseArray<Real> > _zero;
  MooseArray<MooseArray<RealGradient> > _grad_zero;
  MooseArray<MooseArray<RealTensor> > _second_zero;

protected:
  /**
   * A range for use with TBB.  We do this so that it doesn't have
   * to get rebuilt all the time (which takes time).
   */
  ConstElemRange * _active_local_elem_range;

  friend class ComputeInternalJacobians;
  friend class ComputeInternalJacobianBlocks;
  friend class ComputeInternalResiduals;

  friend class PDEBase;
  friend class Kernel;
  friend class AuxKernel;
  friend class BoundaryCondition;
  friend class Material;
  friend class Stabilizer;
  friend class Executioner;
  friend class TransientExecutioner;
  friend class Steady;

  friend class QuadraturePointData;
  friend class ElementData;
  friend class FaceData;
  friend class AuxData;
};

  
#endif //MOOSESYSTEM_H
