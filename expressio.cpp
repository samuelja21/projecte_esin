#include "expressio.hpp"
#include <stack>
#include<cmath>
using namespace std;

   bool expressio::comprova_sintaxis(const list<token> &lt, list<token>::const_iterator it){
      bool sint(true);
      if ((*it).tipus() <= token::VARIABLE){
         ++it;
         if (it == lt.end() or ((*it).tipus() >= token::SUMA and (*it).tipus() <= token::EXPONENCIACIO)) sint = true;
         else if ((*it).tipus() == token::TANCAR_PAR) sint = true;
         else sint = false;
      }
      else if ((*it).tipus() <= token::EXPONENCIACIO){
         ++it;
         if (((*it).tipus() >= token::SUMA and (*it).tipus() <= token::EXPONENCIACIO) or (*it).tipus() == token::TANCAR_PAR) sint = false;
         else sint = true;
      }
      else if ((*it).tipus() <= token::SIGNE_POSITIU){
         ++it;
         if ((*it).tipus() <= token::VARIABLE or (*it).tipus() == token::OBRIR_PAR) sint = true;
         else sint = false;
      }
      else if ((*it).tipus() <= token::EVALF){
         ++it;
         if ((*it).tipus() == token::OBRIR_PAR) sint = true;
         else sint = false;
      }
      return sint;
   }

   expressio::node* expressio::copia_nodes(node* m){
      node* n;
      if (m == nullptr) n = nullptr;
      else{
         n = new node;
         try{
            n->info = m->info;
            n->fe = copia_nodes(m->fe);
            n->fd = copia_nodes(m->fd);
         } catch(...){
            delete n;
            throw;
         }
      }
      return n;
   }

   void expressio::esborra_nodes(node* m){
      if (m != nullptr){
         esborra_nodes(m->fe);
         esborra_nodes(m->fd);
         delete m;
      }
   }

   bool expressio::compara_nodes(node* m, node* n){
      if (m != nullptr and n != nullptr){
         if (m->info == n->info) return (compara_nodes(m->fe, n->fe) and compara_nodes(m->fd, n->fd));
         else return false;
      }
      else if (m == nullptr and n == nullptr) return true;
      else return false;
   }

   void expressio::llista_vars(node* m, list<string> &l){
   	if (m != nullptr){
   		llista_vars(m->fe, l);
   		if (m->info.tipus() == token::VARIABLE) l.insert(l.begin(), m->info.identificador_variable());
   		llista_vars(m->fd, l);
   	}
   }
   
   expressio::node* expressio::substitucio(node* m, const string &v, const expressio e){
   	if (m != nullptr){
   		if (m->info.tipus() == token::VARIABLE){
            m = copia_nodes(e._arrel);
   		}
   		m->fe = substitucio(m->fe, v, e);
   		m->fd = substitucio(m->fd, v, e);
   	}
      return m;
   }
   
   void expressio::llista_tokens(node *m, list<token> &lt){
   	if (m != nullptr){
   		if (m->info.tipus() >= token::SQRT){
   			lt.insert(lt.end(), m->info);
   			lt.insert(lt.end(), token(token::OBRIR_PAR));
   			llista_tokens(m->fd, lt);
   			lt.insert(lt.end(), token(token::TANCAR_PAR));
   		}
   		
   		else if (m->info.tipus() <= token::VAR_PERCENTATGE){
   			lt.insert(lt.end(), m->info);
		   }
			
		   else if (m->info.tipus() <= token::SIGNE_POSITIU){
            if (m->fe->info.tipus() <= token::SIGNE_POSITIU and m->fe->info.tipus() >= token::SUMA and m->info > m->fe->info){
               lt.insert(lt.end(), token(token::OBRIR_PAR));
               llista_tokens(m->fe, lt);
               lt.insert(lt.end(), token(token::TANCAR_PAR));
            }
            else llista_tokens(m->fe, lt);
            
            lt.insert(lt.end(), m->info);
            
            if (m->fd->info.tipus() <= token::SIGNE_POSITIU and m->fd->info.tipus() >= token::SUMA and m->info > m->fd->info){
               lt.insert(lt.end(), token(token::OBRIR_PAR));
               llista_tokens(m->fd, lt);
               lt.insert(lt.end(), token(token::TANCAR_PAR));
            }
            else if ((m->fd->info.tipus() == token::SUMA or m->fd->info.tipus() == token::RESTA) and m->info.tipus() == token::RESTA){
               lt.insert(lt.end(), token(token::OBRIR_PAR));
               llista_tokens(m->fd, lt);
               lt.insert(lt.end(), token(token::TANCAR_PAR));
            }
            
            else if ((m->fd->info.tipus() == token::MULTIPLICACIO or m->fd->info.tipus() == token::DIVISIO) and m->info.tipus() == token::DIVISIO){
               lt.insert(lt.end(), token(token::OBRIR_PAR));
               llista_tokens(m->fd, lt);
               lt.insert(lt.end(), token(token::TANCAR_PAR));
            }
            
            else llista_tokens(m->fd, lt);	
		   }
   	}
   }

   expressio::node* expressio::simplificar(node* m, bool &s){
      s = false;
      if (m->info.tipus() == token::CT_RACIONAL){
         if (m->info.valor_racional().denom() == 1){
            m->info = token(m->info.valor_racional().num());
            s = true;
         }
      }
      else if (m->info.tipus() == token::SUMA){
         if ((m->fe->info.tipus() < token::VARIABLE and m->fd->info.tipus() < token::VARIABLE)){
            token n;
            if (m->fd->info.tipus() == token::CT_ENTERA){
               if (m->fe->info.tipus() == token::CT_ENTERA) n = token(m->fe->info.valor_enter() + m->fd->info.valor_enter());
               else if (m->fe->info.tipus() == token::CT_REAL) n = token(m->fe->info.valor_real() + m->fd->info.valor_enter());
               else if (m->fe->info.tipus() == token::CT_RACIONAL) n = token(m->fe->info.valor_racional() + racional(m->fd->info.valor_enter()));
            }
            else if (m->fd->info.tipus() == token::CT_REAL){
               if (m->fe->info.tipus() == token::CT_ENTERA) n = token(m->fe->info.valor_enter() + m->fd->info.valor_real());
               else if (m->fe->info.tipus() == token::CT_REAL) n = token(m->fe->info.valor_real() + m->fd->info.valor_real());
               else if (m->fe->info.tipus() == token::CT_RACIONAL) n = token(double(m->fe->info.valor_racional().num()) / double(m->fe->info.valor_racional().denom()) + m->fd->info.valor_real());
            }
            else if (m->fd->info.tipus() == token::CT_RACIONAL){
               if (m->fe->info.tipus() == token::CT_ENTERA) n = token(racional(m->fe->info.valor_enter()) + m->fd->info.valor_racional());
               else if (m->fe->info.tipus() == token::CT_REAL) n = token(m->fe->info.valor_real() + double(m->fd->info.valor_racional().num()) / double(m->fd->info.valor_racional().denom()));
               else if (m->fe->info.tipus() == token::CT_RACIONAL) n = token(m->fe->info.valor_racional() + m->fd->info.valor_racional());
            }
            m->info = n;
            s = true;
            m->fe = nullptr;
            m->fd = nullptr;
         }
         else if (m->fe->info == token(0)){
               m = m->fd;
               s = true;
         }
         else if (m->fd->info == token(0)){
               m = m->fe;
               s = true;
         } 
         else if (compara_nodes(m->fe, m->fd)){
               node *n = new node;
               n->info = token(token::MULTIPLICACIO);
               node *n2 = new node;
               n2->info = token(2);
               n->fe = n2;
               n->fd = m->fd;
               m = n;
               s = true;
         }
         else if (m->fd->info.tipus() == token::CANVI_DE_SIGNE){
            node *n = new node;
            n->info = token(token::RESTA);
            n->fe = m->fe;
            n->fd = m->fd->fd;
            m = n;
            s = true;
         }
         else if (m->fe->info.tipus() == token::CANVI_DE_SIGNE){
            node *n = new node;
            n->info = token(token::RESTA);
            n->fe = m->fd;
            n->fd = m->fe->fd;
            m = n;
            s = true;
         }
         else if (m->fe->info.tipus() == token::DIVISIO and m->fd->info.tipus() == token::DIVISIO){
            if (compara_nodes(m->fe->fd, m->fd->fd)){
               node *n = new node;
               n->info = token(token::DIVISIO);
               n->fd = m->fe->fd;
               node *n1 = new node;
               n1->info = token(token::SUMA);
               n1->fe = m->fe->fe;
               n1->fd = m->fd->fe;
               n->fe = n1;
               m = n;
               s = true;
            }
         }
         else if (m->fe->info.tipus() == token::MULTIPLICACIO and m->fd->info.tipus() == token::MULTIPLICACIO){
            if (compara_nodes(m->fe->fd, m->fd->fd)){
               node *n = new node;
               n->info = token(token::MULTIPLICACIO);
               n->fd = m->fe->fd;
               node *n1 = new node;
               n1->info = token(token::SUMA);
               n1->fe = m->fe->fe;
               n1->fd = m->fd->fe;
               n->fe = n1;
               m = n;
               s = true;
            }
            else if (compara_nodes(m->fe->fd, m->fd->fe)){
               node *n = new node;
               n->info = token(token::MULTIPLICACIO);
               n->fd = m->fe->fd;
               node *n1 = new node;
               n1->info = token(token::SUMA);
               n1->fe = m->fe->fe;
               n1->fd = m->fd->fd;
               n->fe = n1;
               m = n;
               s = true;
            }
            else if (compara_nodes(m->fe->fe, m->fd->fe)){
               node *n = new node;
               n->info = token(token::MULTIPLICACIO);
               n->fd = m->fe->fe;
               node *n1 = new node;
               n1->info = token(token::SUMA);
               n1->fe = m->fe->fd;
               n1->fd = m->fd->fd;
               n->fe = n1;
               m = n;
               s = true;
            }
            else if (compara_nodes(m->fe->fe, m->fd->fd)){
               node *n = new node;
               n->info = token(token::MULTIPLICACIO);
               n->fd = m->fe->fe;
               node *n1 = new node;
               n1->info = token(token::SUMA);
               n1->fe = m->fe->fd;
               n1->fd = m->fd->fe;
               n->fe = n1;
               m = n;
               s = true;
            }
         }
      }
      else if (m->info.tipus() == token::RESTA){
         if ((m->fe->info.tipus() < token::VARIABLE and m->fd->info.tipus() < token::VARIABLE)){
            token n;
            if (m->fd->info.tipus() == token::CT_ENTERA){
               if (m->fe->info.tipus() == token::CT_ENTERA) n = token(m->fe->info.valor_enter() - m->fd->info.valor_enter());
               else if (m->fe->info.tipus() == token::CT_REAL) n = token(m->fe->info.valor_real() - m->fd->info.valor_enter());
               else if (m->fe->info.tipus() == token::CT_RACIONAL) n = token(m->fe->info.valor_racional() - racional(m->fd->info.valor_enter()));
            }
            else if (m->fd->info.tipus() == token::CT_REAL){
               if (m->fe->info.tipus() == token::CT_ENTERA) n = token(m->fe->info.valor_enter() - m->fd->info.valor_real());
               else if (m->fe->info.tipus() == token::CT_REAL) n = token(m->fe->info.valor_real() - m->fd->info.valor_real());
               else if (m->fe->info.tipus() == token::CT_RACIONAL) n = token(double(m->fe->info.valor_racional().num()) / double(m->fe->info.valor_racional().denom()) - m->fd->info.valor_real());
            }
            else if (m->fd->info.tipus() == token::CT_RACIONAL){
               if (m->fe->info.tipus() == token::CT_ENTERA) n = token(racional(m->fe->info.valor_enter()) - m->fd->info.valor_racional());
               else if (m->fe->info.tipus() == token::CT_REAL) n = token(m->fe->info.valor_real() - double(m->fd->info.valor_racional().num()) / double(m->fd->info.valor_racional().denom()));
               else if (m->fe->info.tipus() == token::CT_RACIONAL) n = token(m->fe->info.valor_racional() - m->fd->info.valor_racional());
            }
            m->info = n;
            s = true;
            m->fe = nullptr;
            m->fd = nullptr;
         }
         else if (m->fe->info == token(0)){
               node *n = new node;
               n->info = token(token::CANVI_DE_SIGNE);
               n->fd = m->fd;
               m = n;
               s = true;
         }
         else if (m->fd->info == token(0)){
               m = m->fe;
               s = true;
         } 
         else if (compara_nodes(m->fe, m->fd)){
               node *n = new node;
               n->info = token(0);
               m = n;
               s = true;
         }
         else if (m->fd->info.tipus() == token::CANVI_DE_SIGNE){
            node *n = new node;
            n->info = token(token::SUMA);
            n->fe = m->fe;
            n->fd = m->fd->fd;
            m = n;
            s = true;
         }
         else if (m->fe->info.tipus() == token::DIVISIO and m->fd->info.tipus() == token::DIVISIO){
            if (compara_nodes(m->fe->fd, m->fd->fd)){
               node *n = new node;
               n->info = token(token::DIVISIO);
               n->fd = m->fe->fd;
               node *n1 = new node;
               n1->info = token(token::RESTA);
               n1->fe = m->fe->fe;
               n1->fd = m->fd->fe;
               n->fe = n1;
               m = n;
               s = true;
            }
         }
         else if (m->fe->info.tipus() == token::MULTIPLICACIO and m->fd->info.tipus() == token::MULTIPLICACIO){
            if (compara_nodes(m->fe->fd, m->fd->fd)){
               node *n = new node;
               n->info = token(token::MULTIPLICACIO);
               n->fd = m->fe->fd;
               node *n1 = new node;
               n1->info = token(token::RESTA);
               n1->fe = m->fe->fe;
               n1->fd = m->fd->fe;
               n->fe = n1;
               m = n;
               s = true;
            }
            else if (compara_nodes(m->fe->fd, m->fd->fe)){
               node *n = new node;
               n->info = token(token::MULTIPLICACIO);
               n->fd = m->fe->fd;
               node *n1 = new node;
               n1->info = token(token::RESTA);
               n1->fe = m->fe->fe;
               n1->fd = m->fd->fd;
               n->fe = n1;
               m = n;
               s = true;
            }
            else if (compara_nodes(m->fe->fe, m->fd->fe)){
               node *n = new node;
               n->info = token(token::MULTIPLICACIO);
               n->fd = m->fe->fe;
               node *n1 = new node;
               n1->info = token(token::RESTA);
               n1->fe = m->fe->fd;
               n1->fd = m->fd->fd;
               n->fe = n1;
               m = n;
               s = true;
            }
            else if (compara_nodes(m->fe->fe, m->fd->fd)){
               node *n = new node;
               n->info = token(token::MULTIPLICACIO);
               n->fd = m->fe->fe;
               node *n1 = new node;
               n1->info = token(token::RESTA);
               n1->fe = m->fe->fd;
               n1->fd = m->fd->fe;
               n->fe = n1;
               m = n;
               s = true;
            }
         }
      }
      else if (m->info.tipus() == token::CANVI_DE_SIGNE){
         if (m->fd->info.tipus() <= token::CT_REAL){
            node *n = new node;
            if (m->fd->info.tipus() == token::CT_RACIONAL){
               n ->info = token(racional(m->fd->info.valor_racional().num() * -1, m->fd->info.valor_racional().denom()));
            }
            else if (m->fd->info.tipus() == token::CT_REAL) n->info = token(-1 * m->fd->info.valor_real());
            else n->info = token(-1 * m->fd->info.valor_enter());
            m = n;
            s = true;
         }
         else if (m->fd->info.tipus() == token::CANVI_DE_SIGNE){
            m = m->fd->fd;
            s = true;
         }
      }
      else if (m->info.tipus() == token::SIGNE_POSITIU){
         m = m->fd;
         s = true;
      }
      else if (m->info.tipus() == token::MULTIPLICACIO){
         if ((m->fe->info.tipus() < token::VARIABLE and m->fd->info.tipus() < token::VARIABLE)){
            token n;
            if (m->fd->info.tipus() == token::CT_ENTERA){
               if (m->fe->info.tipus() == token::CT_ENTERA) n = token(m->fe->info.valor_enter() * m->fd->info.valor_enter());
               else if (m->fe->info.tipus() == token::CT_REAL) n = token(m->fe->info.valor_real() * m->fd->info.valor_enter());
               else if (m->fe->info.tipus() == token::CT_RACIONAL) n = token(m->fe->info.valor_racional() * racional(m->fd->info.valor_enter()));
            }
            else if (m->fd->info.tipus() == token::CT_REAL){
               if (m->fe->info.tipus() == token::CT_ENTERA) n = token(m->fe->info.valor_enter() * m->fd->info.valor_real());
               else if (m->fe->info.tipus() == token::CT_REAL) n = token(m->fe->info.valor_real() * m->fd->info.valor_real());
               else if (m->fe->info.tipus() == token::CT_RACIONAL) n = token(double(m->fe->info.valor_racional().num()) / double(m->fe->info.valor_racional().denom()) * m->fd->info.valor_real());
            }
            else if (m->fd->info.tipus() == token::CT_RACIONAL){
               if (m->fe->info.tipus() == token::CT_ENTERA) n = token(racional(m->fe->info.valor_enter()) * m->fd->info.valor_racional());
               else if (m->fe->info.tipus() == token::CT_REAL) n = token(m->fe->info.valor_real() * double(m->fd->info.valor_racional().num()) / double(m->fd->info.valor_racional().denom()));
               else if (m->fe->info.tipus() == token::CT_RACIONAL) n = token(m->fe->info.valor_racional() * m->fd->info.valor_racional());
            }
            m->info = n;
            s = true;
            m->fe = nullptr;
            m->fd = nullptr;
         }
         else if (m->fd->info == token(1)) m = m->fe;
         else if (m->fe->info == token(1)) m = m->fd;
         else if (m->fe->info == token(0) or m->fd->info == token(0)){
               node *n = new node;
               n->info = token(0);
               m = n;
               s = true;
         }
         else if (compara_nodes(m->fe, m->fd)){
            node *n = new node;
            n->info = token(token::EXPONENCIACIO);
            n->fe = m->fe;
            node *n1 = new node;
            n1->info = token(2);
            n->fd = n1;
            m = n;
            s = true;
         }
         else if (m->fd->info.tipus() == token::CANVI_DE_SIGNE){
            node *n1 = new node;
            n1->info = token(token::MULTIPLICACIO);
            n1->fe = m->fe;
            n1->fd = m->fd->fd;
            node *n = new node;
            n->info = token(token::CANVI_DE_SIGNE);
            n->fd = n1;
            m = n;
            s = true;
         }
         else if (m->fe->info.tipus() == token::CANVI_DE_SIGNE){
            node *n1 = new node;
            n1->info = token(token::MULTIPLICACIO);
            n1->fe = m->fd;
            n1->fd = m->fe->fd;
            node *n = new node;
            n->info = token(token::CANVI_DE_SIGNE);
            n->fd = n1;
            m = n;
            s = true;
         }
         else if (m->fe->info.tipus() == token::DIVISIO){
            if (m->fe->fe->info == token(1)){
               node *n = new node;
               n->info = token(token::DIVISIO);
               n->fe = m->fd;
               n->fd = m->fe->fd;
               m = n;
               s = true;
            }
         }
         else if (m->fd->info.tipus() == token::DIVISIO){
            if (m->fd->fe->info == token(1)){
               node *n = new node;
               n->info = token(token::DIVISIO);
               n->fe = m->fe;
               n->fd = m->fd->fd;
               m = n;
               s = true;
            }
         }
         else if (m->fe->info.tipus() == token::EXPONENCIACIO and m->fd->info.tipus() == token::EXPONENCIACIO){
            if (compara_nodes(m->fe->fd, m->fd->fd)){
               node *n1 = new node;
               n1->info = token(token::MULTIPLICACIO);
               n1->fe = m->fe->fe;
               n1->fd = m->fd->fe;
               node *n = new node;
               n->info = token(token::EXPONENCIACIO);
               n->fe = n1;
               n->fd = m->fe->fd;
               m = n;
               s = true;
            }
            else if (compara_nodes(m->fe->fe, m->fd->fe)){
               node *n1 = new node;
               n1->info = token(token::SUMA);
               n1->fe = m->fe->fd;
               n1->fd = m->fd->fd;
               node *n = new node;
               n->info = token(token::EXPONENCIACIO);
               n->fd = n1;
               n->fe = m->fe->fe;
               m = n;
               s = true;
            }
         }
         else if (m->fe->info.tipus() == token::EXP and m->fd->info.tipus() == token::EXP){
               node *n = new node;
               n->info = token(token::EXP);
               node *n1 = new node;
               n1->info = token(token::SUMA);
               n1->fe = m->fe->fd;
               n1->fd = m->fd->fd;
               n->fd = n1;
               m = n;
               s = true;
         }
      }
      else if (m->info.tipus() == token::DIVISIO){
         if (m->fd->info == token(0)) throw error(ErrorSintactic);
         if ((m->fe->info.tipus() < token::VARIABLE and m->fd->info.tipus() < token::VARIABLE)){
            token n;
            if (m->fd->info.tipus() == token::CT_ENTERA){
               if (m->fe->info.tipus() == token::CT_ENTERA) n = token(m->fe->info.valor_enter() / m->fd->info.valor_enter());
               else if (m->fe->info.tipus() == token::CT_REAL) n = token(m->fe->info.valor_real() / m->fd->info.valor_enter());
               else if (m->fe->info.tipus() == token::CT_RACIONAL) n = token(m->fe->info.valor_racional() / racional(m->fd->info.valor_enter()));
            }
            else if (m->fd->info.tipus() == token::CT_REAL){
               if (m->fe->info.tipus() == token::CT_ENTERA) n = token(m->fe->info.valor_enter() / m->fd->info.valor_real());
               else if (m->fe->info.tipus() == token::CT_REAL) n = token(m->fe->info.valor_real() / m->fd->info.valor_real());
               else if (m->fe->info.tipus() == token::CT_RACIONAL) n = token(double(m->fe->info.valor_racional().num()) / double(m->fe->info.valor_racional().denom()) / m->fd->info.valor_real());
            }
            else if (m->fd->info.tipus() == token::CT_RACIONAL){
               if (m->fe->info.tipus() == token::CT_ENTERA) n = token(racional(m->fe->info.valor_enter()) / m->fd->info.valor_racional());
               else if (m->fe->info.tipus() == token::CT_REAL) n = token(m->fe->info.valor_real() / double(m->fd->info.valor_racional().num()) / double(m->fd->info.valor_racional().denom()));
               else if (m->fe->info.tipus() == token::CT_RACIONAL) n = token(m->fe->info.valor_racional() / m->fd->info.valor_racional());
            }
            m->info = n;
            s = true;
            m->fe = nullptr;
            m->fd = nullptr;
         }
         else if (m->fe->info == token(0)){
            node *n = new node;
            n->info = token(0),
            m = n;
            s = true;
         }
         else if (m->fd->info == token(1)){
            m = m->fe;
            s = true;
         }
         else if (compara_nodes(m->fe, m->fd)){
            node *n = new node;
            n->info = token(1);
            m = n;
            s = true;
         }
         else if (m->fd->info.tipus() == token::CANVI_DE_SIGNE){
            node *n1 = new node;
            n1->info = token(token::DIVISIO);
            n1->fe = m->fe;
            n1->fd = m->fd->fd;
            node *n = new node;
            n->info = token(token::CANVI_DE_SIGNE);
            n->fd = n1;
            m = n;
            s = true;
         }
         else if (m->fd->info.tipus() == token::DIVISIO){
            if (m->fd->fe->info == token(1)){
               node *n = new node;
               n->info = token(token::MULTIPLICACIO);
               n->fe = m->fe;
               n->fd = m->fd->fd;
               m = n;
               s = true;
            }
         }
         else if (m->fe->info.tipus() == token::EXPONENCIACIO and m->fd->info.tipus() == token::EXPONENCIACIO){
            if (compara_nodes(m->fe->fd, m->fd->fd)){
               node *n1 = new node;
               n1->info = token(token::DIVISIO);
               n1->fe = m->fe->fe;
               n1->fd = m->fd->fe;
               node *n = new node;
               n->info = token(token::EXPONENCIACIO);
               n->fe = n1;
               n->fd = m->fe->fd;
               m = n;
               s = true;
            }
            else if (compara_nodes(m->fe->fe, m->fd->fe)){
               node *n1 = new node;
               n1->info = token(token::RESTA);
               n1->fe = m->fe->fd;
               n1->fd = m->fd->fd;
               node *n = new node;
               n->info = token(token::EXPONENCIACIO);
               n->fd = n1;
               n->fe = m->fe->fe;
               m = n;
               s = true;
            }
         }
         else if (m->fe->info.tipus() == token::EXP and m->fd->info.tipus() == token::EXP){
               node *n = new node;
               n->info = token(token::EXP);
               node *n1 = new node;
               n1->info = token(token::RESTA);
               n1->fe = m->fe->fd;
               n1->fd = m->fd->fd;
               n->fd = n1;
               m = n;
               s = true;
         }
      }
      else if (m->info.tipus() == token::EXPONENCIACIO){
         if (m->fe->info.tipus() < token::VARIABLE and m->fd->info.tipus() < token::VARIABLE){
            if (m->fd->info.tipus() != token::CT_ENTERA){
               if (m->fe->info.tipus() == token::CT_ENTERA and m->fe->info.valor_enter() < 0) throw error(ErrorSintactic);
               else if (m->fe->info.tipus() == token::CT_REAL and m->fe->info.valor_real() < 0) throw error(ErrorSintactic);
               else if (m->fe->info.tipus() == token::CT_RACIONAL and m->fe->info.valor_racional().num() < 0) throw error(ErrorSintactic);
            }
            token t;
            if (m->fe->info.tipus() < token::VARIABLE and m->fd->info.tipus() == token::CT_ENTERA){
               if (m->fe->info.tipus() == token::CT_ENTERA) t = token(pow(m->fe->info.valor_enter(), m->fd->info.valor_enter()));
               else if (m->fe->info.tipus() == token::CT_REAL) t = token(pow(m->fe->info.valor_real(), m->fd->info.valor_enter()));
               else if (m->fe->info.tipus() == token::CT_RACIONAL) t = token(pow(double(m->fd->info.valor_racional().num()) / double(m->fd->info.valor_racional().denom()) , m->fd->info.valor_enter()));
               node* n = new node;
               n->info = t;
               m = n;
               s = true;
            }
            else if (m->fe->info.tipus() == token::CT_REAL and m->fd->info.tipus() == token::CT_REAL){
               t = token(pow(m->fe->info.valor_real(), m->fd->info.valor_real()));
               node* n = new node;
               n->info = t;
               m = n;
               s = true;            
            }
         }
         else if (m->fd->info == token(0)){
            node *n = new node;
            n->info = token(1);
            m = n;
            s = true;
         }
         else if (m->fd->info == token(1)){
            m = m->fe;
            s = true;
         }
         else if (m->fd->info.tipus() == token::CANVI_DE_SIGNE){
            node* n1 = new node;
            n1->info = token(token::EXPONENCIACIO);
            n1->fe = m->fe;
            n1->fd = m->fd->fd;
            node* n = new node;
            node* n2 = new node;
            n2->info = token(1);
            n->fe = n2;
            n->info = token(token::DIVISIO);
            n->fd = n1;
            m = n;
            s = true;
         }
         else if (m->fe->info.tipus() == token::EXPONENCIACIO){
            node *n1 = new node;
            n1->info = token(token::MULTIPLICACIO);
            n1->fe = m->fe->fd;
            n1->fd = m->fd;
            node *n = new node;
            n->info = token(token::EXPONENCIACIO);
            n->fe = m->fe->fe;
            n->fd = n1;
            m = n;
            s = true;
         }
         else if (m->fe->info.tipus() == token::EXP){
            node *n1 = new node;
            n1->info = token(token::MULTIPLICACIO);
            n1->fe = m->fe->fd;
            n1->fd = m->fd;
            node *n = new node;
            n->info = token(token::EXP);
            n->fd = n1;
            m = n;
            s = true;
         }
      }
      else if (m->info.tipus() == token::SQRT){
         if (m->fd->info.tipus() < token::VARIABLE){
               token t;
               if (m->fd->info.tipus() == token::CT_ENTERA){
                  if (m->fd->info.valor_enter() < 0) throw error(ErrorSintactic);
                  else t = token(sqrt(m->fd->info.valor_enter()));
               }
               else if (m->fd->info.tipus() == token::CT_REAL){
                  if ( m->fd->info.valor_real() < 0) throw error(ErrorSintactic);
                  else t = token(sqrt(m->fd->info.valor_real()));
               } 
               else if (m->fd->info.tipus() == token::CT_RACIONAL){
                  if ( m->fd->info.valor_racional().num() < 0) throw error(ErrorSintactic);
                  else t = token(sqrt(double(m->fd->info.valor_racional().num()) / double(m->fd->info.valor_racional().denom())));
               }
               node *n = new node;
               n->info = t,
               m = n;
               s = true;
         }
         else{
            node *n = new node;
            n->info = token(token::EXPONENCIACIO);
            n->fe = m->fd;
            node *n1 = new node;
            n1->info = token(racional(1,2));
            n->fd = n1;
            m = n;
            s = true;
         }
      }
      else if(m->info.tipus() == token::EXP){
         if (m->fd->info.tipus() < token::VARIABLE){
            token t;
            if (m->fd->info.tipus() == token::CT_ENTERA) t = token(exp(m->fd->info.valor_enter()));
            else if (m->fd->info.tipus() == token::CT_REAL) t = token(exp(m->fd->info.valor_real()));
            else if (m->fd->info.tipus() == token::CT_RACIONAL) t = token(exp(double(m->fd->info.valor_racional().num()) / double(m->fd->info.valor_racional().denom())));
            node *n = new node;
            n->info = t;
            m = n;
            s = true;
         }
         else if (m->fd->info.tipus() == token::LOG){
            m = m->fd->fd;
            s = true;
         }
      }
      else if(m->info.tipus() == token::LOG){
         if (m->fd->info.tipus() < token::VARIABLE){
            token t;
            if (m->fd->info.tipus() == token::CT_ENTERA){
               if (m->fd->info.valor_enter() < 0) throw error(ErrorSintactic);
               else t = token(log(m->fd->info.valor_enter()));
            }
            else if (m->fd->info.tipus() == token::CT_REAL){
               if ( m->fd->info.valor_real() < 0) throw error(ErrorSintactic);
               else t = token(log(m->fd->info.valor_real()));
            }
            else if (m->fd->info.tipus() == token::CT_RACIONAL){
               if ( m->fd->info.valor_racional().num() < 0) throw error(ErrorSintactic);
               else t = token(log(double(m->fd->info.valor_racional().num()) / double(m->fd->info.valor_racional().denom())));
            }
            node *n = new node;
            n->info = t;
            m = n;
            s = true;
         }
         else if (m->fd->info.tipus() == token::EXP){
            m = m->fd->fd;
            s = true;
         }
         else if (m->fd->info.tipus() == token::MULTIPLICACIO){
            node *n = new node;
            n->info = token(token::SUMA);
            node *n1 = new node;
            node *n2 = new node;
            n1->info = token(token::LOG);
            n2->info = token(token::LOG);
            n1->fd = m->fd->fe;
            n2->fd = m->fd->fd;
            n->fe = n1;
            n->fd = n2;
            m = n;
            s = true;
         }
         else if (m->fd->info.tipus() == token::DIVISIO){
            node *n = new node;
            n->info = token(token::RESTA);
            node *n1 = new node;
            node *n2 = new node;
            n1->info = token(token::LOG);
            n2->info = token(token::LOG);
            n1->fd = m->fd->fe;
            n2->fd = m->fd->fd;
            n->fe = n1;
            n->fd = n2;
            m = n;
            s = true;
         }
         else if (m->fd->info.tipus() == token::EXPONENCIACIO){
            node *n = new node;
            n->info = token(token::MULTIPLICACIO);
            node *n1 = new node;
            n1->info = token(token::LOG);
            n1->fd = m->fd->fe;
            n->fd = n1;
            n->fe = m->fd->fd;
            m = n;
            s = true;
         }
      }
      else if(m->info.tipus() == token::EVALF){
         token t;
         if (m->fd->info.tipus() < token::VARIABLE){
            if (m->fd->info.tipus() == token::CT_ENTERA) t = token(double(m->fd->info.valor_enter()));
            else if (m->fd->info.tipus() == token::CT_REAL) t = token(m->fd->info.valor_real());
            else if (m->fd->info.tipus() == token::CT_RACIONAL) t = token(double(m->fd->info.valor_racional().num()) / double(m->fd->info.valor_racional().denom()));
            node *n = new node;
            n->info = t;
            m = n;
            s = true;
         }
         else if (m->fd->info.tipus() == token::CT_E){
            t = token(2.718281828);
            node *n = new node;
            n->info = t;
            m = n;
            s = true;
         }
         else if (m->fd->info.tipus() == token::VARIABLE){
            t = token(m->fd->info.identificador_variable());
            node *n = new node;
            n->info = t;
            m = n;
            s = true;
         }
         else if (m->fd->info.tipus() >= token::SUMA and m->fd->info.tipus() <= token::EXPONENCIACIO){
            node *n = new node;
            n->info = m->fd->info;
            node* n1 = new node;
            n1->info = token(token::EVALF);
            n1->fd = m->fd->fe;
            node* n2 = new node;
            n2->info = token(token::EVALF),
            n2->fd = m->fd->fd;
            n->fe = simplificar(n1, s);
            n->fd = simplificar(n2, s);
            m = n;
            s = true;
         }
         else if (m->fd->info.tipus() >= token::CANVI_DE_SIGNE and m->fd->info.tipus() <= token::LOG){
            node* n = new node;
            n->info = m->fd->info;
            node* n1 = new node;
            n1->info = token(token::EVALF);
            n1->fd = m->fd->fd;
            n->fd = simplificar(n1, s);
            m = n;
            s = true;
         }
      }
      return m;
   }

   expressio::node* expressio::simplificacio(node* m){
      bool s = true;
      if (m != nullptr){
         while(s){
            m->fe = simplificacio(m->fe);
            m->fd = simplificacio(m->fd);
            m = simplificar(m, s);
         }
      }
      return m;
   }
 
 /* Constructora d'una expressió formada per un sol token: un operand. Si
     s'utiliza el valor del token per defecte es construeix la que
     anomenem "expressió buida". Si el tipus del token no és el del token
     per defecte (NULLTOK), ni el d'una CT_ENTERA, CT_RACIONAL, CT_REAL,
     CT_E, VARIABLE o VAR_PERCENTAtGE es produeix un error sintàctic. */
    expressio::expressio(const token t) throw(error){
        if (t.tipus() > token::VAR_PERCENTATGE) throw error(ErrorSintactic);
        else{
         _arrel = new node;
         _arrel->info = t;
        }
  }

  /* Constructora a partir d'una seqüència de tokens. Es produeix un error si
     la seqüència és buida o si no es pot construir l'arbre d'expressió
     corresponent(és a dir, si és sintàcticament incorrecta). */
   expressio::expressio(const list<token> & l) throw(error){
      if (l.size() == 0) throw error(ErrorSintactic);
      else{
         list<token>::const_iterator it = l.begin();
         stack<token> op;
         stack<node*> ex;
         bool par(false);
         while(it != l.end()){
            if (comprova_sintaxis(l, it)){
               if ((*it).tipus() <= token::VARIABLE){
                  node *a = new node;
                  a->info = *it;
                  a->fd = nullptr;
                  a->fe = nullptr;
                  ex.push(a);
               }
               else if ((*it).tipus() <= token::EXPONENCIACIO){
                  if (not op.empty() and op.top().tipus() != token::OBRIR_PAR){
                     if ((*it) < op.top()){
                        while (not op.empty()){
                           node *a = new node;
                           a->info = op.top();
                           a->fd = ex.top();
                           ex.pop();
                           a->fe = ex.top();
                           ex.pop();
                           ex.push(a);
                           op.pop();
                        } 
                     }
                  }
                  op.push(*it);
               }

               else if ((*it).tipus() == token::TANCAR_PAR){
                  par = true;
                  while (par){
                     if (par and op.empty()) throw error(ErrorSintactic);
                     if (op.top().tipus() == token::OBRIR_PAR){
                        op.pop();
                        par = false;
                     }
                     else{
                        node *a = new node;
                        a->info = op.top();
                        a->fd = ex.top();
                        ex.pop();
                        a->fe = ex.top();
                        ex.pop();
                        ex.push(a);
                        op.pop();
                     }
                  }
                  if (not op.empty() and (op.top().tipus() >= token::CANVI_DE_SIGNE and op.top().tipus() <= token::EVALF)){
                     node *a = new node;
                     a->info = op.top();
                     a->fd = ex.top();
                     ex.pop();
                     ex.push(a);
                     op.pop();
                  } 
               }

               else if ((*it).tipus() >= token::CANVI_DE_SIGNE){
                  op.push(*it);
               }
            }
            else throw error(ErrorSintactic);
            ++it;
         }
         while (not op.empty()){
            if (op.top().tipus() == token::OBRIR_PAR) throw error(ErrorSintactic);

            if (op.top().tipus() >= token::CANVI_DE_SIGNE and op.top().tipus() <= token::EVALF){
               node *a = new node;
               a->info = op.top();
               a->fd = ex.top();
               ex.pop();
               ex.push(a);
               op.pop();
            }
            else{
               node *a = new node;
               a->info = op.top();
               a->fd = ex.top();
               ex.pop();
               a->fe = ex.top();
               ex.pop();
               ex.push(a);
               op.pop();
            }
         }
         _arrel = ex.top();  
      }
   }

  // Constructora per còpia, assignació i destructora.
  expressio::expressio(const expressio & e) throw(error){
   _arrel = copia_nodes(e._arrel);
  }

  expressio & expressio::operator=(const expressio & e) throw(error){
   if (this != &e){
      node *aux;
      aux = copia_nodes(e._arrel);
      esborra_nodes(_arrel);
      _arrel = aux;
   }
   return *this;
  }
  expressio::~expressio() throw(error){
   esborra_nodes(_arrel);
  }

  // Retorna cert si i només si s'aplica a l'expressió buida.
  expressio::operator bool() const throw(){
   return _arrel == nullptr; 
  }

  /* Operadors d'igualtat i desigualtat. Dues expressions es consideren
     iguals si i només si els seus arbres d'expressió són idèntics. */
  bool expressio::operator==(const expressio & e) const throw(){
      return compara_nodes(_arrel, e._arrel);
  }
  bool expressio::operator!=(const expressio & e) const throw(){
      return not compara_nodes(_arrel, e._arrel);
  }

  /* Retorna una llista sense repeticions, en qualsevol ordre, amb
     els noms de les variables de l'expressió. */
  void expressio::vars(list<string> & l) const throw(error){
      llista_vars(_arrel, l);
  }

  /* Substitueix totes les aparicions de la variable de nom v per
     l'expressió e. Si no existeix la variable v dins de l'expressió a la
     que apliquem aquest mètode l'expressió no es modifica. */
  void expressio::apply_substitution(const string & v, const expressio & e) throw(error){
      _arrel = substitucio(_arrel, v, e);
      
  }

  /* Aplica un pas de simplificació a l'expressió. La subexpressió a
     simplificar es busca seguint el recorregut "left to right" postordre
     explicat a l'apartat "Procés d'avaluació". Es pot produir qualsevol dels
     errors semàntics que apareixen més avall numerats des del 32 al 35. */
  void expressio::simplify_one_step() throw(error){
      bool s;
      _arrel = simplificar(_arrel, s);
  }

  /* Aplica successius passos de simplificació com l'anterior fins que
     l'expressió es trobi completament simplificada. Llavors diem que es
     troba en "forma normal". Es pot produir qualsevol dels errors
     semàntics que apareixen més avall numerats des del 32 al 35. */
  void expressio::simplify() throw(error){
   _arrel = simplificacio(_arrel);
  }

  /* Converteix l'expressió en la seqüència de tokens lt corresponent: els
     operadors apareixen entre els operands si són infixos, abans si són
     prefixos i els símbols de funció van seguits de parèntesis que tanquen
     els seus arguments separats per comes. S'han d'introduir només aquells
     parèntesis que siguin estrictament necessaris per trencar les regles de
     precedència o associativitat en l'ordre d'aplicació dels operadors. */
  void expressio::list_of_tokens(list<token> & lt) throw(error){
  	llista_tokens(_arrel, lt);
  }


