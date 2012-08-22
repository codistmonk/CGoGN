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
#include <boost/thread.hpp>
#include <boost/thread/barrier.hpp>
#include <vector>

namespace CGoGN
{

namespace Algo
{

namespace Parallel
{

class ThreadFunctionAttrib
{
protected:
	std::vector<unsigned int>& m_ids;
	boost::barrier& m_sync1;
	boost::barrier& m_sync2;
	bool& m_finished;
	unsigned int m_id;
	FunctorAttribThreaded* m_functor;

public:
	ThreadFunctionAttrib(FunctorAttribThreaded* func, std::vector<unsigned int>& vid, boost::barrier& s1, boost::barrier& s2, bool& finished, unsigned int id):
		m_ids(vid), m_sync1(s1), m_sync2(s2), m_finished(finished), m_id(id), m_functor(func)
	{
	}

	ThreadFunctionAttrib(const ThreadFunctionAttrib& tf):
		m_ids(tf.m_ids), m_sync1(tf.m_sync1), m_sync2(tf.m_sync2), m_finished(tf.m_finished), m_id(tf.m_id), m_functor(tf.m_functor){}

	void operator()()
	{
		while (!m_finished)
		{
			for (std::vector<unsigned int>::const_iterator it = m_ids.begin(); it != m_ids.end(); ++it)
				m_functor->run(*it,m_id);
			m_sync1.wait();
			m_sync2.wait();
		}
	}
};




/**
 *
 */
template<typename MAP>
class ThreadFunction
{
protected:
	std::vector<Dart>& m_darts;
	boost::barrier& m_sync1;
	boost::barrier& m_sync2;
	bool& m_finished;
	unsigned int m_id;
	FunctorMapThreaded<MAP>* m_functor;
public:
	ThreadFunction(FunctorMapThreaded<MAP>* func, std::vector<Dart>& vd, boost::barrier& s1, boost::barrier& s2, bool& finished, unsigned int id):
		m_darts(vd), m_sync1(s1), m_sync2(s2), m_finished(finished), m_id(id), m_functor(func)
	{
	}

	ThreadFunction(const ThreadFunction<MAP>& tf):
		m_darts(tf.m_darts), m_sync1(tf.m_sync1), m_sync2(tf.m_sync2), m_finished(tf.m_finished), m_id(tf.m_id), m_functor(tf.m_functor){}

	void operator()()
	{
		while (!m_finished)
		{
			for (std::vector<Dart>::const_iterator it = m_darts.begin(); it != m_darts.end(); ++it)
				m_functor->run(*it,m_id);
			m_sync1.wait();
			m_sync2.wait();
		}
	}
};


inline unsigned int nbThreads()
{
	return boost::thread::hardware_concurrency();
}



inline unsigned int optimalNbThreads()
{
	unsigned int nb = nbThreads();
	if (nb>4)
		return nb/2 ;

	return nb;
}


template <typename MAP, unsigned int ORBIT>
void foreach_cell(MAP& map, FunctorMapThreaded<MAP>& func, bool shared, unsigned int nbth, bool needMarkers, const FunctorSelect& good, unsigned int currentThread)
{
	if (nbth == 0)
		nbth = optimalNbThreads();

	std::vector<FunctorMapThreaded<MAP>*> funcs;
	funcs.reserve(nbth);

	if (shared)
	{
		for (unsigned int i = 0; i < nbth; ++i)
			funcs.push_back(&func);
	}
	else
	{
		for (unsigned int i = 0; i < nbth; ++i)
			funcs.push_back(func.duplicate());
	}


	foreach_cell<MAP,ORBIT>(map,funcs,nbth,needMarkers,good,currentThread);

	if (!shared)
		for (unsigned int i = 0; i < nbth; ++i)
			delete funcs[i];
}

template <typename MAP, unsigned int ORBIT>
void foreach_cell(MAP& map, std::vector<FunctorMapThreaded<MAP>*>& funcs, unsigned int nbth, bool needMarkers, const FunctorSelect& good, unsigned int currentThread)
{
	assert(funcs.size() ==  nbth);

	std::vector<Dart>* vd = new std::vector<Dart>[nbth];
	boost::thread** threads = new boost::thread*[nbth];

	// nbth new functions, new thread (with good darts !)
	for (unsigned int i = 0; i < nbth; ++i)
		vd[i].reserve(SIZE_BUFFER_THREAD);


	AttributeContainer* cont = NULL;
	DartMarker* dmark = NULL;
	CellMarker<ORBIT>* cmark = NULL;
	AttributeMultiVector<Dart>* quickTraversal = map.template getQuickTraversal<ORBIT>() ;

	// fill each vd buffers with SIZE_BUFFER_THREAD darts
	Dart d;
	unsigned int di=0;

	if(quickTraversal != NULL)
	{
		cont = &(map.template getAttributeContainer<ORBIT>()) ;

		di = cont->begin();
		unsigned int nb = 0;
		while ((di != cont->end()) && (nb < nbth*SIZE_BUFFER_THREAD) )
		{
			d = quickTraversal->operator[](di);
			if (good(d))
			{
				vd[nb%nbth].push_back(d);
				nb++;
			}
			cont->next(di);
		}
	}
	else
	{
		if(map.template isOrbitEmbedded<ORBIT>())
		{
			cmark = new CellMarker<ORBIT>(map, currentThread) ;

			d = map.begin();
			unsigned int nb = 0;
			while ((d != map.end()) && (nb < nbth*SIZE_BUFFER_THREAD) )
			{
				if (good(d) && (!cmark->isMarked(d)))
				{
					cmark->mark(d);
					vd[nb%nbth].push_back(d);
					nb++;
				}
				map.next(d);
			}
		}
		else
		{
			dmark = new DartMarker(map, currentThread) ;
			d = map.begin();
			unsigned int nb = 0;
			while ((d != map.end()) && (nb < nbth*SIZE_BUFFER_THREAD) )
			{
				if (good(d) && (!dmark->isMarked(d)))
				{
					dmark->markOrbit<ORBIT>(d);
					vd[nb%nbth].push_back(d);
					nb++;
				}
				map.next(d);
			}
		}
	}

	boost::barrier sync1(nbth+1);
	boost::barrier sync2(nbth+1);
	bool finished=false;
	// lauch threads
	if (needMarkers)
	{
		unsigned int nbth_prec = map.getNbThreadMarkers();
		if (nbth_prec < nbth+1)
			map.addThreadMarker(nbth+1-nbth_prec);
	}

	for (unsigned int i = 0; i < nbth; ++i)
		threads[i] = new boost::thread(ThreadFunction<MAP>(funcs[i], vd[i],sync1,sync2, finished,1+i));

	// and continue to traverse the map
	std::vector<Dart>* tempo = new std::vector<Dart>[nbth];

	for (unsigned int i = 0; i < nbth; ++i)
		tempo[i].reserve(SIZE_BUFFER_THREAD);


	if (cont)
	{
		while (di != cont->end())
		{
			for (unsigned int i = 0; i < nbth; ++i)
				tempo[i].clear();
			unsigned int nb = 0;
			while ((di != cont->end()) && (nb < nbth*SIZE_BUFFER_THREAD) )
			{
				d = quickTraversal->operator[](di);
				if (good(d))
				{
					tempo[nb%nbth].push_back(d);
					nb++;
				}
				cont->next(di);
			}
			sync1.wait();
			for (unsigned int i = 0; i < nbth; ++i)
				vd[i].swap(tempo[i]);
			sync2.wait();
		}
	}
	else if (cmark)
	{
		while (d != map.end())
		{
			for (unsigned int i = 0; i < nbth; ++i)
				tempo[i].clear();
			unsigned int nb = 0;
			while ((d != map.end()) && (nb < nbth*SIZE_BUFFER_THREAD) )
			{
				if (good(d) && (!cmark->isMarked(d)))
				{
					cmark->mark(d);
					tempo[nb%nbth].push_back(d);
					nb++;
				}
				map.next(d);
			}
			sync1.wait();
			for (unsigned int i = 0; i < nbth; ++i)
				vd[i].swap(tempo[i]);
			sync2.wait();
		}
	}
	else
	{
		while (d != map.end())
		{
			for (unsigned int i = 0; i < nbth; ++i)
				tempo[i].clear();
			unsigned int nb = 0;
			while ((d != map.end()) && (nb < nbth*SIZE_BUFFER_THREAD) )
			{
				if (good(d) && (!dmark->isMarked(d)))
				{
					dmark->markOrbit<ORBIT>(d);
					tempo[nb%nbth].push_back(d);
					nb++;
				}
				map.next(d);
			}
			sync1.wait();
			for (unsigned int i = 0; i < nbth; ++i)
				vd[i].swap(tempo[i]);
			sync2.wait();
		}
	}

	sync1.wait();
	finished = true;
	sync2.wait();

	//wait for all theads to be finished
	for (unsigned int i = 0; i < nbth; ++i)
	{
		threads[i]->join();
		delete threads[i];
	}
	delete[] threads;
	delete[] vd;
	delete[] tempo;

	if (cmark != NULL)
		delete cmark;

	if (dmark != NULL)
		delete dmark;
}



template <typename MAP>
void foreach_dart(MAP& map, FunctorMapThreaded<MAP>& func, bool shared, unsigned int nbth, bool needMarkers, const FunctorSelect& good)
{
	if (nbth == 0)
		nbth = optimalNbThreads();

	std::vector<FunctorMapThreaded<MAP>*> funcs;
	funcs.reserve(nbth);

	if (shared)
	{
		for (unsigned int i = 0; i < nbth; ++i)
			funcs.push_back(&func);
	}
	else
	{
		for (unsigned int i = 0; i < nbth; ++i)
			funcs.push_back(func.duplicate());
	}

	foreach_dart<MAP>(map,funcs,nbth,needMarkers,good);

	if (!shared)
		for (unsigned int i = 0; i < nbth; ++i)
			delete funcs[i];
}


template <typename MAP>
void foreach_dart(MAP& map, std::vector<FunctorMapThreaded<MAP>*> funcs, unsigned int nbth, bool needMarkers, const FunctorSelect& good)
{
	assert(funcs.size() ==  nbth);

	std::vector<Dart>* vd = new std::vector<Dart>[nbth];
	boost::thread** threads = new boost::thread*[nbth];

	Dart d = map.begin();

	// nbth new functions, new thread (with good darts !)
	for (unsigned int i = 0; i < nbth; ++i)
		vd[i].reserve(SIZE_BUFFER_THREAD);

	// fill each vd buffers with 4096 darts
	unsigned int nb = 0;
	while ((d != map.end()) && (nb < nbth*SIZE_BUFFER_THREAD) )
	{
		if (good(d))
		{
			vd[nb%nbth].push_back(d);
			nb++;
		}
		map.next(d);
	}

	boost::barrier sync1(nbth+1);
	boost::barrier sync2(nbth+1);
	bool finished = false;
	// lauch threads
	if (needMarkers)
	{
		unsigned int nbth_prec = map.getNbThreadMarkers();
		if (nbth_prec < nbth+1)
			map.addThreadMarker(nbth+1-nbth_prec);
	}

	for (unsigned int i = 0; i < nbth; ++i)
	{
//		funcs[i]->setThreadID(1+i);
		threads[i] = new boost::thread(ThreadFunction<MAP>(funcs[i], vd[i],sync1,sync2, finished,1+i));
	}

	// and continue to traverse the map
	std::vector<Dart>* tempo = new std::vector<Dart>[nbth];
	for (unsigned int i = 0; i < nbth; ++i)
		tempo[i].reserve(SIZE_BUFFER_THREAD);

	while (d != map.end())
	{
		for (unsigned int i = 0; i < nbth; ++i)
			tempo[i].clear();

		unsigned int nb =0;
		while ((d != map.end()) && (nb < nbth*SIZE_BUFFER_THREAD) )
		{
			if (good(d))
			{
				tempo[nb%nbth].push_back(d);
				nb++;
			}
			map.next(d);
		}

		sync1.wait();
		for (unsigned int i = 0; i < nbth; ++i)
			vd[i].swap(tempo[i]);
		sync2.wait();
	}

	sync1.wait();
	finished = true;
	sync2.wait();

	//wait for all theads to be finished
	for (unsigned int i = 0; i < nbth; ++i)
	{
		threads[i]->join();
		delete threads[i];
	}
	
	delete vd;
	delete threads;
	delete tempo;
}



inline void foreach_attrib(AttributeContainer& attr_cont, FunctorAttribThreaded& func, bool shared, unsigned int nbth)
{
	if (nbth == 0)
		nbth = optimalNbThreads();

	std::vector<FunctorAttribThreaded*> funcs;
	funcs.reserve(nbth);

	if (shared)
	{
		for (unsigned int i = 0; i < nbth; ++i)
			funcs.push_back(&func);
	}
	else
	{
		for (unsigned int i = 0; i < nbth; ++i)
			funcs.push_back(func.duplicate());
	}

	foreach_attrib(attr_cont,funcs,nbth);

	if (!shared)
		for (unsigned int i = 0; i < nbth; ++i)
			delete funcs[i];

}


inline void foreach_attrib(AttributeContainer& attr_cont, std::vector<FunctorAttribThreaded*> funcs, unsigned int nbth)
{
	assert(funcs.size() ==  nbth);

	std::vector<unsigned int >* vid = new std::vector<unsigned int>[2*nbth];
	boost::thread** threads = new boost::thread*[nbth];

	for (unsigned int i = 0; i < 2*nbth; ++i)
		vid[i].reserve(SIZE_BUFFER_THREAD);

	// fill each vid buffers with 4096 id
	unsigned int id = attr_cont.begin();
	unsigned int nb = 0;
	unsigned int nbm = nbth*SIZE_BUFFER_THREAD;
	while ((id != attr_cont.end()) && (nb < nbm))
	{
		vid[nb%nbth].push_back(id);
		nb++;
		attr_cont.next(id);
	}


	boost::barrier sync1(nbth+1);
	boost::barrier sync2(nbth+1);
	bool finished=false;
	// lauch threads
	for (unsigned int i = 0; i < nbth; ++i)
		threads[i] = new boost::thread(ThreadFunctionAttrib(funcs[i], vid[i],sync1,sync2, finished,1+i));

	while (id != attr_cont.end())
	{
		for (unsigned int i = nbth; i < 2*nbth; ++i)
			vid[i].clear();

		unsigned int nb = 0;
		while ((id != attr_cont.end()) && (nb < nbm))
		{
			vid[nbth + nb%nbth].push_back(id);
			nb++;
			attr_cont.next(id);
		}

		sync1.wait();
		for (unsigned int i = 0; i < nbth; ++i)
			vid[i].swap(vid[nbth+i]);
		sync2.wait();
	}

	sync1.wait();
	finished = true;
	sync2.wait();

	//wait for all theads to be finished
	for (unsigned int i = 0; i < nbth; ++i)
	{
		threads[i]->join();
		delete threads[i];
	}
	delete[] threads;
	delete[] vid;
}



// TODO same modification for transparent usage of dart marker / cell marker / quick traversal

template <typename MAP, unsigned int CELL>
void foreach_cell2Pass(MAP& map, std::vector<FunctorMapThreaded<MAP>*>& funcsFrontnBack, unsigned int nbLoops, unsigned int nbth, bool needMarkers, const FunctorSelect& good)
{
	std::vector<Dart>* vd = new std::vector<Dart>[2*nbth];
	for (unsigned int i = 0; i < nbth; ++i)
		vd[i].reserve(SIZE_BUFFER_THREAD);

	boost::thread** threadsAB = new boost::thread*[2*nbth];

	if (needMarkers)
	{
		// ensure that there is enough threads
		unsigned int nbth_prec = map.getNbThreadMarkers();
		if (nbth_prec < nbth+1)
			map.addThreadMarker(nbth+1-nbth_prec);
	}

	CellMarkerNoUnmark<CELL> cm(map); // for 2 pass front mark / back unmark

	boost::barrier sync1(nbth+1);
	boost::barrier sync2(nbth+1);

	for (unsigned int loop=0; loop< nbLoops; ++loop)
	{
		// PASS FRONT (A)
		{
			Dart d = map.begin();
			// fill each vd buffers with SIZE_BUFFER_THREAD darts
			unsigned int nb = 0;
			while ((d != map.end()) && (nb < nbth*SIZE_BUFFER_THREAD) )
			{
				if (good(d) && (!cm.isMarked(d)))
				{
					cm.mark(d);
					vd[nb%nbth].push_back(d);
					nb++;
				}
				map.next(d);
			}

			bool finished=false;
			// lauch threads funcsFrontnBack

			for (unsigned int i = 0; i < nbth; ++i)
				threadsAB[i] = new boost::thread(ThreadFunction<MAP>(funcsFrontnBack[i], vd[i], sync1, sync2, finished,1+i));

			// and continue to traverse the map

			while (d != map.end())
			{
				for (unsigned int i = 0; i < nbth; ++i)
					vd[nbth+i].clear();

				unsigned int nb = 0;
				while ((d != map.end()) && (nb < nbth*SIZE_BUFFER_THREAD) )
				{
					if (good(d) && (!cm.isMarked(d)))
					{
						cm.mark(d);
						vd[nbth+nb%nbth].push_back(d);
						nb++;
					}
					map.next(d);
				}

				sync1.wait();
				for (unsigned int i = 0; i < nbth; ++i)
					vd[i].swap(vd[nbth+i]);
				sync2.wait();
			}

			sync1.wait();
			finished = true;
			sync2.wait();

			//wait for all theads to be finished
			for (unsigned int i = 0; i < nbth; ++i)
				threadsAB[i]->join();
		}
		// PASS BACK (B)
		{
			for (unsigned int i = 0; i < nbth; ++i)
				vd[i].clear();
			Dart d = map.begin();
			// fill each vd buffers with SIZE_BUFFER_THREAD darts
			unsigned int nb = 0;
			while ((d != map.end()) && (nb < nbth*SIZE_BUFFER_THREAD) )
			{
				if (good(d) && (cm.isMarked(d)))
				{
					cm.unmark(d);
					vd[nb%nbth].push_back(d);
					nb++;
				}
				map.next(d);
			}

			bool finished=false;
			for (unsigned int i = 0; i < nbth; ++i)
				threadsAB[nbth+i] = new boost::thread(ThreadFunction<MAP>(funcsFrontnBack[nbth+i], vd[i],sync1,sync2, finished,1+i));

			// and continue to traverse the map

			while (d != map.end())
			{
				for (unsigned int i = 0; i < nbth; ++i)
					vd[nbth+i].clear();

				unsigned int nb = 0;
				while ((d != map.end()) && (nb < nbth*SIZE_BUFFER_THREAD) )
				{
					if (good(d) && (cm.isMarked(d)))
					{
						cm.unmark(d);
						vd[nbth+nb%nbth].push_back(d);
						nb++;
					}
					map.next(d);
				}

				sync1.wait();
				for (unsigned int i = 0; i < nbth; ++i)
					vd[i].swap(vd[nbth+i]);
				sync2.wait();
			}

			sync1.wait();
			finished = true;
			sync2.wait();

			//wait for all theads to be finished
			for (unsigned int i = 0; i < nbth; ++i)
				threadsAB[nbth+i]->join();
		}
	}

	// free buffers and threads
	for (unsigned int i = 0; i < 2*nbth; ++i)
	{
		delete threadsAB[i];
	}
	delete[] threadsAB;
	delete[] vd;
}



template <typename MAP, unsigned int CELL>
void foreach_cell2Pass(MAP& map, FunctorMapThreaded<MAP>& funcFront, FunctorMapThreaded<MAP>& funcBack, bool shared, unsigned int nbLoops, unsigned int nbth, bool needMarkers, const FunctorSelect& good)
{
	if (nbth == 0)
		nbth = optimalNbThreads();

	std::vector<FunctorMapThreaded<MAP>*> funcs;
	funcs.reserve(nbth);

	if (shared)
	{
		for (unsigned int i = 0; i < nbth; ++i)
			funcs.push_back(&funcFront);
		for (unsigned int i = 0; i < nbth; ++i)
			funcs.push_back(&funcBack);
	}
	else
	{
		for (unsigned int i = 0; i < nbth; ++i)
			funcs.push_back(funcFront.duplicate());
		for (unsigned int i = 0; i < nbth; ++i)
			funcs.push_back(funcBack.duplicate());
	}


	foreach_cell2Pass<MAP,CELL>(map,funcs,nbLoops,nbth,needMarkers,good);

	if (!shared)
		for (unsigned int i = 0; i < nbth; ++i)
			delete funcs[i];

}


} // namespace Parallel

} // namespace Algo

} // namespace CGoGN
