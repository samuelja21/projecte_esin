#include "expressio.hpp"
#include <stack>
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
         return (compara_nodes(m->fe, n->fe) and compara_nodes(m->fd, n->fd));
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
                     if (op.top().tipus() > (*it).tipus()){
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
                  if (not op.empty() and (op.top().tipus() >= token::SQRT and op.top().tipus() <= token::EVALF)){
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

            if (op.top().tipus() >= token::SQRT and op.top().tipus() <= token::EVALF){
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

  }

  /* Aplica successius passos de simplificació com l'anterior fins que
     l'expressió es trobi completament simplificada. Llavors diem que es
     troba en "forma normal". Es pot produir qualsevol dels errors
     semàntics que apareixen més avall numerats des del 32 al 35. */
  void expressio::simplify() throw(error){

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


