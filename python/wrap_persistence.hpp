template< typename Complex, typename Cell_less>
std::vector<std::size_t> 
run_persistence( Complex & complex,
                 Cell_less & less){
   typedef ctl::Graded_cell_complex< Complex, Cell_less > Filtration;
   //Boundary Operator
   //NOTE: This is not a general purpose boundary operator.
   //It works correctly only when successive 
   //boundaries are taken in a filtration order
   //typedef typename Filtration::iterator Filtration_iterator;
   typedef ctl::Graded_boundary< Filtration> Filtration_boundary;
   typedef typename Filtration::Term Filtration_term;
   typedef typename Filtration_term::Coefficient Coefficient;

   typedef typename ctl::Sparse_matrix< Coefficient> Sparse_matrix;
   typedef typename ctl::Offset_map< typename Filtration::iterator> Offset_map;
   typedef typename ctl::Sparse_matrix_map< Coefficient, Offset_map> Chain_map;
   
   ctl::Timer timer;
   
   //produce a filtration
   timer.start();
   Filtration complex_filtration( complex);
   Filtration_boundary filtration_boundary( complex_filtration);
   double complex_filtration_time = timer.elapsed();
   //display some info
   std::cout << "time to compute complex filtration (secs): "
             << complex_filtration_time << std::endl;

   //begin instantiate our vector of cascades homology
   Sparse_matrix R( complex.size());

   Offset_map offset_map( complex_filtration.begin());
   //we hand persistence a property map for genericity!                        
   Chain_map R_map( R.begin(), offset_map);
   timer.start();
   auto times = ctl::persistence( complex_filtration.begin(),
                                  complex_filtration.end(),
                                  filtration_boundary,
                                  R_map, false, offset_map);
   double boundary_map_build = times.first;
   double complex_persistence = times.second;
  
   std::cout << "initialize_cascade_data (complex): "
            << boundary_map_build << std::endl;
   std::cout << "serial persistence (complex): "
             << complex_persistence << std::endl;
   std::cout << "total time : " << timer.elapsed() << std::endl;
   std::vector< std::size_t> bti;
   compute_betti( complex_filtration, R_map, bti, true);
   return bti;
}


template< typename Complex>
std::vector<std::size_t> 
compute_homology( Complex & complex){
  typedef ctl::Cell_less Complex_cell_less;
  Complex_cell_less less;
  return run_persistence( complex, less);
}

std::vector< std::size_t> homology( const std::list< std::list< int>>& simplices){
	ctl::Cell_complex< ctl::Simplex_boundary> cell_complex;
        for( auto& s : simplices){
		ctl::Abstract_simplex sigma( s.begin(), s.end());
		cell_complex.insert_closed_cell( sigma);
	}
	std::cout << cell_complex << std::endl;	
	return compute_homology(cell_complex);

}


// Creates a Python class for an `Abstract_simplex`. 
void wrap_persistence(py::module &mod){
  mod.def("homology", &homology, "compute the homology of the list of simplices");
}
