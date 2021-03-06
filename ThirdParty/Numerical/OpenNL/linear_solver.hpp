/*******************************************************************************
* CGoGN: Combinatorial and Geometric modeling with Generic N-dimensional Maps  *
* version 0.1                                                                  *
* Copyright (C) 2009-2012, IGG Team, LSIIT, University of Strasbourg           *
*                                                                              *
* This library is free software; you can redistribute it and/or modify it      *
* under the terms of the GNU Lesser General Public License as published by the *
* Free Software Foundation; either version 2.1 of the License, or (at your     *
* option) any later version.                                                   *
*                                                                              *
* This library is distributed in the hope that it will be useful, but WITHOUT  *
* ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or        *
* FITNESS FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License  *
* for more details.                                                            *
*                                                                              *
* You should have received a copy of the GNU Lesser General Public License     *
* along with this library; if not, write to the Free Software Foundation,      *
* Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA.           *
*                                                                              *
* Web site: http://cgogn.unistra.fr/                                           *
* Contact information: cgogn@unistra.fr                                        *
*                                                                              *
*******************************************************************************/

template <class PARAMS>
LinearSolver<PARAMS>::LinearSolver(unsigned int nb_variables) {
	least_squares_ = false ;
	direct_ = false ;
	matrix_already_set_ = false ;
	nb_variables_ = nb_variables ;
	variable_ = new Variable<CoeffType>[nb_variables] ;
	state_ = INITIAL ;
	A_ = NULL ;
	x_ = NULL ;
	b_ = NULL ;
	direct_solver_ = new DirectSolver() ;
}

template <class PARAMS>
void LinearSolver<PARAMS>::begin_system() {
	current_row_ = 0 ;
	transition(INITIAL, IN_SYSTEM) ;
	
	if(!matrix_already_set_) {
		// Enumerate free variables.
		unsigned int index = 0 ;
		for(unsigned int i = 0; i < nb_variables() ; i++) {
			Variable<CoeffType>& v = variable(i) ;
			if(!v.is_locked()) {
				v.set_index(index) ;
				index++ ;
			}
		}

		// Size of the actual system to solve
		unsigned int n = index ;

		// Allocate and init internal structures
		if(direct_)
			A_ = new Matrix(n, Matrix::COLUMNS) ;
		else
			A_ = new Matrix(n) ;
		x_ = new Vector(n) ;
		b_ = new Vector(n) ;
		for(unsigned int i = 0; i < n; i++) {
			(*x_)[i] = 0 ;
			(*b_)[i] = 0 ;
		}
	}

	variables_to_vector() ;
}

template <class PARAMS>
void LinearSolver<PARAMS>::begin_row() {
	transition(IN_SYSTEM, IN_ROW) ;
	af_.clear() ;
	if_.clear() ;
	al_.clear() ;
	xl_.clear() ;
	bk_ = 0 ;
}

template <class PARAMS>
void LinearSolver<PARAMS>::set_right_hand_side(CoeffType b) {
	check_state(IN_ROW) ;
	bk_ = b ;
}

template <class PARAMS>
void LinearSolver<PARAMS>::add_coefficient(unsigned int iv, CoeffType a) {
	check_state(IN_ROW) ;
	Variable<CoeffType>& v = variable(iv) ;
	if(v.is_locked()) {
		al_.push_back(a) ;
		xl_.push_back(v.value()) ;
	} else {
		af_.push_back(a) ;
		if_.push_back(v.index()) ;
	}
}

template <class PARAMS>
void LinearSolver<PARAMS>::normalize_row(CoeffType weight) {
	check_state(IN_ROW) ;
	CoeffType norm = 0.0 ;
	unsigned int nf = af_.size() ;
	for(unsigned int i=0; i<nf; i++) {
		norm += af_[i] * af_[i] ;
	}
	unsigned int nl = al_.size() ;
	for(unsigned int i=0; i<nl; i++) {
		norm += al_[i] * al_[i] ;
	}
	norm = sqrt(norm) ;
	scale_row(weight / norm) ;
}

template <class PARAMS>
void LinearSolver<PARAMS>::scale_row(CoeffType s) {
	check_state(IN_ROW) ;
	unsigned int nf = af_.size() ;
	for(unsigned int i=0; i<nf; i++) {
		af_[i] *= s ;
	}
	unsigned int nl = al_.size() ;
	for(unsigned int i=0; i<nl; i++) {
		al_[i] *= s ;
	}
	bk_ *= s ; 
}

template <class PARAMS>
void LinearSolver<PARAMS>::end_row() {
	if(least_squares_) {
		unsigned int nf = af_.size() ;
		unsigned int nl = al_.size() ;
		// Construct the matrix coefficients of the current row
		if(!matrix_already_set_) {
			for(unsigned int i = 0; i < nf; i++) {
				for(unsigned int j = 0; j < nf; j++) {
					A_->add(if_[i], if_[j], af_[i] * af_[j]) ;
				}
			}
		}
		// Construct the right-hand side of the current row
		CoeffType S = - bk_ ;
		for(unsigned int j = 0; j < nl; j++) {
			S += al_[j] * xl_[j] ;
		}
		for(unsigned int i = 0; i < nf; i++) {
			(*b_)[if_[i]] -= af_[i] * S ;
		}
	} else {
		unsigned int nf = af_.size() ;
		unsigned int nl = al_.size() ;
		// Construct the matrix coefficients of the current row
		if(!matrix_already_set_) {
			for(unsigned int i = 0; i < nf; i++) {
				A_->add(current_row_, if_[i], af_[i]) ;
			}
		}
		// Construct the right-hand side of the current row
		(*b_)[current_row_] = bk_ ;
		for(unsigned int i = 0; i < nl; i++) {
			(*b_)[current_row_] -= al_[i] * xl_[i] ;
		}
	}
	current_row_++ ;
	transition(IN_ROW, IN_SYSTEM) ;
}

template <class PARAMS>
void LinearSolver<PARAMS>::end_system() {
	if(direct_ && !direct_solver_->factorized()) {
		direct_solver_->factorize(*A_) ;
	}
    transition(IN_SYSTEM, CONSTRUCTED) ;
}

template <class PARAMS>
void LinearSolver<PARAMS>::solve() {
	check_state(CONSTRUCTED) ;
	if(least_squares_)
	{
		if(direct_) {
			direct_solver_->solve(*b_, *x_) ;
		} else {
			SymmetricSolver solver ;
			solver.solve(*A_, *b_, *x_) ;
		}
	} else {
		NonSymmetricSolver solver ;
		solver.solve(*A_, *b_, *x_) ;
	}
	vector_to_variables() ;
	transition(CONSTRUCTED, SOLVED) ;
}

template <class PARAMS>
void LinearSolver<PARAMS>::reset(bool keep_matrix) {
    if(keep_matrix) {
    	unsigned int n = x_->dimension() ;
    	for(unsigned int i=0; i<n; i++) {
    		(*x_)[i] = 0 ;
    		(*b_)[i] = 0 ;
    	}
    	matrix_already_set_ = true ;
    } else {
    	delete A_ ; A_ = NULL ;
    	delete x_ ; x_ = NULL ;
    	delete b_ ; b_ = NULL ;
		direct_solver_->reset() ;
    	matrix_already_set_ = false ;
    	for(unsigned int i = 0; i < nb_variables_; ++i) {
    		variable_[i].unlock() ;
    	}
    }
	state_ = INITIAL ;
}

template <class PARAMS>
void LinearSolver<PARAMS>::vector_to_variables() {
	for(unsigned int i=0; i < nb_variables(); i++) {
	Variable<CoeffType>& v = variable(i) ;
		if(!v.is_locked()) {
			v.set_value((*x_)[v.index()]) ;
		}
	}
}

template <class PARAMS>
void LinearSolver<PARAMS>::variables_to_vector() {
	for(unsigned int i=0; i < nb_variables(); i++) {
	Variable<CoeffType>& v = variable(i) ;
		if(!v.is_locked()) {
			(*x_)[v.index()] = v.value() ;
		}
	}
}

