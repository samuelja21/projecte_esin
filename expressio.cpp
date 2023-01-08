#include "expressio.hpp"
#include <stack>
#include<cmath>
using namespace std;

   //Pre: tenim dos tokens(operadors) t1 i t2.
   /*Post: retorna true si els dos operadors t1 i t2 tenen la mateixa prioritat i són operadors binaris, és a dir, qualsevol combinació
    entre SUMA i RESTA o DIVISIÓ i MULTIPLICACIÓ.*/
   bool expressio::operadors(token t1, token t2){
      return ((t1.tipus() == token::SUMA or t1.tipus() == token::RESTA) and (t2.tipus() == token::SUMA or t2.tipus() == token::RESTA)) or ((t1.tipus() == token::MULTIPLICACIO or t1.tipus() == token::DIVISIO) and (t2.tipus() == token::MULTIPLICACIO or t2.tipus() == token::DIVISIO));
   }

   //Pre: tenim una llista i un iterador d'aquesta llista.
   /*Post: retorna true només si no hi ha cap error sintactic en el següent token de la llista, que comporti una incompatibilitat amb l'actual, 
   en aquest cas tornaria false.*/
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

   //Pre: Tenim un node m;
   //Post: Retorna un node amb la mateixa informació que el node m,  s'utilitza a la constructora per còpia.
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

   //Pre: Tenim un node m
   //Post: Elimina el node m i tots els seus subnodes. S'utilitza en la destructora.
   void expressio::esborra_nodes(node* m){
      if (m != nullptr){
         esborra_nodes(m->fe);
         esborra_nodes(m->fd);
         delete m;
      }
   }

   //Pre: Tenim dos nodes m i n.
   //Post: Retorna true si el node m i el node n són exactament iguals, és a dir, si tenen els mateixos fills, els mateixos valors.
   bool expressio::compara_nodes(node* m, node* n){
      if (m != nullptr and n != nullptr){
         if (m->info == n->info) return (compara_nodes(m->fe, n->fe) and compara_nodes(m->fd, n->fd));
         else return false;
      }
      else if (m == nullptr and n == nullptr) return true;
      else return false;
   }

   //Pre: Tenim un node m i una llista de variables
   /*Post: Es recorre l'arbre en inordre, si troba una variable, l'afegeix a la llista, en cas que ja hi sigui, la ignora i continua. 
   Retorna la llista amb totes les variables de l'arbre.*/
   void expressio::llista_vars(node* m, list<string> &l){
   	if (m != nullptr){
   		llista_vars(m->fe, l);
   		if (m->info.tipus() == token::VARIABLE){
            bool trobat(true);
            list<string>::iterator it = l.begin();
            while(it != l.end() and trobat){
               if ((*it) == m->info.identificador_variable()) trobat = false;
               ++it;
            }
            if (trobat) l.insert(l.begin(), m->info.identificador_variable());
         }
   		llista_vars(m->fd, l);
   	}
   }

   //Pre: Tenim un node m (arbre), un string v (variable) i una expressio e.
   /*Post: Recorre l'arbre m en preordre, si troba un token que és una variable amb nom v, llavors substitueix el node per l'expressio e. 
   Retorna l'arbre amb totes les variables v canviades per l'expressio e.*/
   expressio::node* expressio::substitucio(node* m, const string &v, const expressio e){
   	if (m != nullptr){
   		if (m->info.tipus() == token::VARIABLE){
            if (m->info.identificador_variable() == v) m = copia_nodes(e._arrel);
   		}
   		m->fe = substitucio(m->fe, v, e);
   		m->fd = substitucio(m->fd, v, e);
   	}
      return m;
   }

   //Pre: Tenim un node op, arbre.
   /*Post: Comprova tots els casos on hi pot haver un error quan simplifiquem una expressio, si troba algun llançarà l'error, 
   en cas contrari acabarà sense fer cap canvi.*/
   void expressio::comprova_errors(node* op){
      if (op->info.tipus() == token::DIVISIO and (op->fd->info.tipus() == token::CT_ENTERA and op->fd->info.valor_enter() == 0)) throw error(DivPerZero);
      else if (op->info.tipus() == token::EXPONENCIACIO and (op->fe->info.tipus() < token::VARIABLE and op->fd->info.tipus() < token::VARIABLE)){
         if (op->fd->info.tipus() != token::CT_ENTERA){
            if (op->fe->info.tipus() == token::CT_ENTERA and op->fe->info.valor_enter() < 0) throw error(NegatElevNoEnter);
            else if (op->fe->info.tipus() == token::CT_REAL and op->fe->info.valor_real() < 0) throw error(NegatElevNoEnter);
            else if (op->fe->info.tipus() == token::CT_RACIONAL and op->fe->info.valor_racional().num() < 0) throw error(NegatElevNoEnter);
         }
      }
      else if (op->info.tipus() == token::SQRT){
         if (op->fd->info.tipus() == token::CT_ENTERA and op->fd->info.valor_enter() < 0) throw error(SqrtDeNegatiu);
         if (op->fd->info.tipus() == token::CT_REAL and op->fd->info.valor_real() < 0) throw error(SqrtDeNegatiu);
         if (op->fd->info.tipus() == token::CT_RACIONAL and op->fd->info.valor_racional().num() < 0) throw error(SqrtDeNegatiu);
      }
      else if (op->info.tipus() == token::LOG){
         if (op->fd->info.tipus() == token::CT_ENTERA and op->fd->info.valor_enter() < 0) throw error(ErrorSintactic);
         else if (op->fd->info.tipus() == token::CT_REAL and op->fd->info.valor_real() < 0) throw error(ErrorSintactic);
         else if (op->fd->info.tipus() == token::CT_RACIONAL and op->fd->info.valor_racional().num() < 0) throw error(ErrorSintactic);
      }
   }

   //Pre: Tenim un node op, arbre.
   /*Post: Retorna un node amb info un token de tipus CT_ENTERA, CT_REAL o CT_RACIONAL, s'utilitza en els casos de simplificació on c1 op c2 i c1 i c2 són del 
   mateix tipus. Si op és exponencial llavors c2 és un enter o c1 i c2 nombres reals. Per als operadors unaris (log, sqrt, exp) el operant és un real.*/
   expressio::node* expressio::operacio_simple(node* op){
      token n;
      if (op->info.tipus() == token::SUMA){
         if (op->fd->info.tipus() == token::CT_ENTERA) n = token(op->fe->info.valor_enter() + op->fd->info.valor_enter());
         else if (op->fd->info.tipus() == token::CT_REAL) n = token(op->fe->info.valor_real() + op->fd->info.valor_real());
         else if (op->fd->info.tipus() == token::CT_RACIONAL) n = token(op->fe->info.valor_racional() + op->fd->info.valor_racional()); 
      }
      else if (op->info.tipus() == token::RESTA){
         if (op->fd->info.tipus() == token::CT_ENTERA) n = token(op->fe->info.valor_enter() - op->fd->info.valor_enter());
         else if (op->fd->info.tipus() == token::CT_REAL) n = token(op->fe->info.valor_real() - op->fd->info.valor_real());
         else if (op->fd->info.tipus() == token::CT_RACIONAL) n = token(op->fe->info.valor_racional() - op->fd->info.valor_racional()); 
      }
      else if (op->info.tipus() == token::MULTIPLICACIO){
         if (op->fd->info.tipus() == token::CT_ENTERA) n = token(op->fe->info.valor_enter() * op->fd->info.valor_enter());
         else if (op->fd->info.tipus() == token::CT_REAL) n = token(op->fe->info.valor_real() * op->fd->info.valor_real());
         else if (op->fd->info.tipus() == token::CT_RACIONAL) n = token(op->fe->info.valor_racional() * op->fd->info.valor_racional()); 
      }
      else if (op->info.tipus() == token::DIVISIO){
         if (op->fd->info.tipus() == token::CT_ENTERA){
            if (op->fe->info.valor_enter() % op->fd->info.valor_enter() == 0) n = token(op->fe->info.valor_enter()/op->fd->info.valor_enter());
            else n = token(racional(op->fe->info.valor_enter(), op->fd->info.valor_enter()));
         }
         else if (op->fd->info.tipus() == token::CT_REAL) n = token(op->fe->info.valor_real() / op->fd->info.valor_real());
         else if (op->fd->info.tipus() == token::CT_RACIONAL) n = token(op->fe->info.valor_racional() / op->fd->info.valor_racional()); 
      }
      else if (op->info.tipus() == token::EXPONENCIACIO){
         if(op->fd->info.tipus() == token::CT_ENTERA){
            if (op->fe->info.tipus() == token::CT_ENTERA) n = token(int(pow(op->fe->info.valor_enter(), op->fd->info.valor_enter())));
            else if (op->fe->info.tipus() == token::CT_REAL) n = token(pow(op->fe->info.valor_real(), op->fd->info.valor_enter()));
            else if (op->fe->info.tipus() == token::CT_RACIONAL) n = token(racional(pow(op->fe->info.valor_racional().num(), op->fd->info.valor_enter()) , pow(op->fe->info.valor_racional().denom(), op->fd->info.valor_enter())));  
         }
         else n = token(pow(op->fe->info.valor_real(), op->fd->info.valor_real()));
      }
      else if (op->info.tipus() == token::SQRT){
         n = token(sqrt(op->fd->info.valor_real()));
      }
      else if (op->info.tipus() == token::EXP){
         n = token(exp(op->fd->info.valor_real()));
      }
      else if (op->info.tipus() == token::EXP){
         n = token(log(op->fd->info.valor_real()));
      }
      node *res = new node(n);
      res->fd = res->fe = nullptr;
      return res;
   }

   //Pre: tenim un node op, arbre.
   /*Post: S'utlitza quan els dos fills d'op són iguals, i no són CT_ENTERA, CT_REAL, CT_RACIONAL. 
   Retorna el node amb la simplificació que correspongui segons el tipus del token d'op.*/
   expressio::node* expressio::operants_iguals(node* op){
      node *n = new node;
      if (op->info.tipus() == token::SUMA){
         n->info = token(token::MULTIPLICACIO);
         node *n2 = new node(token(2));
         n2->fe = n2->fd = nullptr;
         n->fe = n2;
         n->fd = op->fd;
      }
      else if (op->info.tipus() == token::RESTA){
         n->info = token(0);
         n->fe = n->fd = nullptr;
      }
      else if (op->info.tipus() == token::MULTIPLICACIO){
         n->info = token(token::EXPONENCIACIO);
         n->fe = op->fe;
         node* n1 = new node(token(2));
         n1->fe = nullptr;
         n1->fd = nullptr;
         n->fd = n1;
      }
      else if (op->info.tipus() == token::DIVISIO){
         n->info = token(1);
         n->fe = n->fd = nullptr;
      }
      return n;
   }

   //Pre: Tenim un node op, arbre.
   //Post: Per als casos on un dels dos operants és un zero. Retorna el node aplicant-li la simplificació que correspongui.
   expressio::node* expressio::operant_zero(node *op){
      if (op->fd->info == token(0)){
         if (op->info.tipus() == token::MULTIPLICACIO or op->info.tipus() == token::SQRT){
            op->info = token(0);
            esborra_nodes(op->fe), esborra_nodes(op->fd);
            op->fe = op->fd = nullptr;
         }
         else if (op->info.tipus() == token::EXPONENCIACIO){
            op->info = token(1);
            esborra_nodes(op->fe), esborra_nodes(op->fd);
            op->fe = op->fd = nullptr;
         }
         else op = op->fe;
      }
      else if (op->fe->info == token(0)){
         if (op->info.tipus() == token::RESTA){
            op->info = token(token::CANVI_DE_SIGNE);
            esborra_nodes(op->fe);
            op->fe = nullptr;
         }
         else if(op->info.tipus() == token::SUMA){
            op = op->fd;
         }
         else if (op->info.tipus() == token::MULTIPLICACIO){
            op->info = token(0);
            esborra_nodes(op->fe), esborra_nodes(op->fd);
            op->fe = op->fd = nullptr;
         }
         else if (op->info.tipus() == token::DIVISIO){
            op->info = token(0);
            esborra_nodes(op->fe), esborra_nodes(op->fd);
            op->fe = op->fd = nullptr;
         }
      }
      return op;
   }

   //Pre: Tenim un node op, arbre.
   //Post: Per als casos on un dels dos operants és un canvi de signe. Retorna el node aplicant-li la simplificació que correspongui.
   expressio::node* expressio::operant_CanviSigne(node* op){
      node *n = new node;
      if (op->fd->info.tipus() == token::CANVI_DE_SIGNE){
         if (op->info.tipus() == token::SUMA or op->info.tipus() == token::RESTA){
            if (op->info.tipus() == token::SUMA) n->info = token(token::RESTA);
            else if (op->info.tipus() == token::RESTA) n->info = token(token::SUMA);
            n->fe = op->fe;
            n->fd = op->fd->fd;
         }
         else if (op->info.tipus() == token::MULTIPLICACIO or op->info.tipus() == token::DIVISIO){
            node *n1 = new node(op->info);
            n1->fe = op->fe;
            n1->fd = op->fd->fd;
            n->info = token(token::CANVI_DE_SIGNE);
            n->fd = n1;
            n->fe = nullptr;
         }
         else if (op->info.tipus() == token::EXPONENCIACIO){
            node* n1 = new node(op->info);
            n1->fe = op->fe;
            n1->fd = op->fd->fd;
            node* n2 = new node(token(1));
            n2->fe = n2->fd = nullptr;
            n->fe = n2;
            n->info = token(token::DIVISIO);
            n->fd = n1;   
         }
         else if(op->info.tipus() == token::SQRT){
            n->info = op->info;
            n->fe = nullptr;
            node *n1 = new node;
            if (op->fd->fd->info.tipus() == token::CT_ENTERA) n1->info = token(-1 * op->fd->fd->info.valor_enter());
            n1->fe = n1->fd = nullptr;
            n->fd = n1;
         }
      }
      else if (op->fe->info.tipus() == token::CANVI_DE_SIGNE){
         if (op->info.tipus() == token::SUMA){
            n->info = token(token::RESTA);
            n->fe = op->fd;
            n->fd = op->fe->fd;
         }
         else if (op->info.tipus() == token::MULTIPLICACIO){
            node *n1 = new node(token(token::MULTIPLICACIO));
            n1->fd = op->fd;
            n1->fe = op->fe->fd;
            n->info = token(token::CANVI_DE_SIGNE);
            n->fd = n1;
            n->fe = nullptr;
         }
      }
      return n;
   }

   /*Pre: El token tk és l'info que volem per al node principal, i el token tk2 és el que volem per a un dels dos nodes fills, comu es la part comuna que hi ha 
   en els dos fills del node original, esq i dret són els fills del node que volem agermanar amb el node comu. L'enter costat és per indicar si volem el node comu 
   com a fill dret o esquerra.*/
   //Post: Per als casos on un node té una part en comú en els dos fills i es pot aplicar factor comú, retorna el node amb tots els canvis fets.
   expressio::node* expressio::factor_comu(token tk, token tk2, node* comu, node* esq, node* dret, int costat){
      node* n = new node;
      n->info = tk;
      node *n1 = new node(tk2);
      n1->fe = esq;
      n1->fd = dret;
      if (costat == 0){
         n->fd = comu;
         n->fe = n1;
      }
      else {
         n->fe = comu;
         n->fd = n1;
      }
      return n;
   }

   //Pre: Tenim un node op i un bool s.
   /*Post: Cas especial de factor comú per a logaritme. Es comproven els casos on es pot aplicar factor comú, si no hi ha cap, es retornarà op igual que com estava i 
   s serà fals. En cas contrari, s'aplicaran els canvis i s serà true.*/
   expressio::node* expressio::factor_comu_log(node* op, bool &s){
      if (op->fd->info.tipus() == token::MULTIPLICACIO or op->fd->info.tipus() == token::DIVISIO){
         node *n = new node;
         if (op->fd->info.tipus() == token::MULTIPLICACIO) n->info = token(token::SUMA);
         else n->info = token(token::RESTA);
         node *n1 = new node(token(token::LOG));
         node *n2 = new node(token(token::LOG));
         n1->fd = op->fd->fe;
         n2->fd = op->fd->fd;
         n->fe = n1;
         n->fd = n2;
         op = n;
         s = true;
      }
      else if (op->fd->info.tipus() == token::EXPONENCIACIO){
         node *n = new node(token(token::MULTIPLICACIO));
         node *n1 = new node(token(token::LOG));
         n1->fd = op->fd->fe;
         n->fd = n1;
         n->fe = op->fd->fd;
         op = n;
         s = true;
      }
      return op;
   }

   //Pre: tenim un node op, arbre, node dir, que ens indica la direcció on volem fer la comprovació i un bool s.
   /*Post: Per a casos com x^(y+y+y+y) o x *(y+y+y+y) aquesta funció recorre la expressio per a simplificar-la com a x^(2*y+y+y), per exemple. 
   Amb dir indicarem si volem fer aquesta comprovació per op->fe o op->fd, el bool s serà true només si es troba un cas que es pugui simplificar.*/
   expressio::node* expressio::agrupar_operants(node* op, node* dir, bool &s){
      node *i = dir;
      bool fi(false);
      while(i != nullptr and not fi){
         if (i->fe != nullptr and i->fd != nullptr){
            if (i->fe->info.tipus() == token::MULTIPLICACIO and compara_nodes(i->fe->fe, i->fe->fd)){
               i->fe->info = token(token::EXPONENCIACIO);
               i->fe->fd->info = token(2);
               s = true;
            }
            else if (i->fd->info.tipus() == token::MULTIPLICACIO and compara_nodes(i->fd->fe, i->fd->fd)){
               i->fd->info = token(token::EXPONENCIACIO);
               i->fd->fd->info = token(2);
               s = true;
            }
            else if (compara_nodes(i->fe, i->fd)){
               if (i->info.tipus() == token::SUMA){
                  i->info = token(token::MULTIPLICACIO);
                  i->fe->info = token(2);
               }
               else if (i->info.tipus() == token::RESTA){
                  i->info = token(0);
                  esborra_nodes(i->fe), esborra_nodes(i->fd);
                  i->fe = i->fd = nullptr;
               }
               else if (i->info.tipus() == token::DIVISIO){
                  i->info = token(1);
                  esborra_nodes(i->fe), esborra_nodes(i->fd);
                  i->fe = i->fd = nullptr;
               }
               fi = true;
               s = true;
            }
            else if (i->fe->info == token(0)){
               i->info = token(token::CANVI_DE_SIGNE);
               i->fe = nullptr;
               fi = true;
               s = true;
            } 
            else if (i->fd->info == token(0)){
               i->info = i->fe->info;
               i->fd = i->fe = nullptr;
               fi = true;
               s = true;
            } 
         }
         i = i->fe;
      }
      return op;
   }

   //Pre: Tenim un node op, arbre.
   //Post: Per als casos on un operador és una divisió entre 1, retorna op amb els canvis que s'han de fer.
   expressio::node* expressio::divisio_entre_un(node* op){
      node *n = new node;
      if (op->fe->info.tipus() == token::DIVISIO){
         n->info = token(token::DIVISIO);
         n->fe = op->fd;
         n->fd = op->fe->fd;
      }
      else if (op->fd->info.tipus() == token::DIVISIO){
         if (op->info.tipus() == token::MULTIPLICACIO) n->info = token(token::DIVISIO);
         else if (op->info.tipus() == token::DIVISIO) n->info = token(token::MULTIPLICACIO);
         n->fe = op->fe;
         n->fd = op->fd->fd;
      }
      return n;
   }

   //Pre: Tenim un node m, arbre, i un bool s.
   /*Post: Primer es comprova que no hi hagi qualsevol error a l'arbre que impideixi la seva simplificació, després es comproven tots els casos on es pot 
   simplificar i si compleix es fan els canvis, al final retornarà l'arbre m amb els canvis, si s'ha fet aglun. S será true en cas d'haver-se simplificar l'arbre, 
   en cas contrari fals.*/
   expressio::node* expressio::simplificar(node* m, bool &s){
      comprova_errors(m);
      s = false;
      if (m->info.tipus() == token::CT_RACIONAL){
         if (m->info.valor_racional().denom() == 1){
            m->info = token(m->info.valor_racional().num());
            s = true;
         }
      }
      else if (m->info.tipus() >= token::SUMA and m->info.tipus() <= token::EXPONENCIACIO){
         if ((m->fe->info.tipus() < token::VARIABLE and m->fd->info.tipus() == m->fe->info.tipus()) and m->info.tipus() != token::EXPONENCIACIO) 
            m = operacio_simple(m), s = true;

         else if ((m->info.tipus() >= token::MULTIPLICACIO) and m->fd->info == token(1)) 
            m = m->fe, s = true;

         else if (m->info.tipus() == token::MULTIPLICACIO and m->fe->info == token(1)) 
            m = m->fd;

         else if(m->fe->info == token(0) and m->info.tipus() != token::EXPONENCIACIO) 
            m = operant_zero(m), s = true;

         else if(m->fd->info == token(0)) 
            m = operant_zero(m), s = true;

         else if ((m->info.tipus() >= token::MULTIPLICACIO) and (m->fd->info.tipus() == token::SUMA or m->fd->info.tipus() == token::RESTA)) 
            m = agrupar_operants(m, m->fd, s);

         else if ((m->info.tipus() >= token::MULTIPLICACIO) and (m->fe->info.tipus() == token::SUMA or m->fe->info.tipus() == token::RESTA)) 
            m = agrupar_operants(m, m->fe, s);

         else if (compara_nodes(m->fe, m->fd) and m->info.tipus() != token::EXPONENCIACIO) 
            m = operants_iguals(m), s = true;

         else if (m->fd->info.tipus() == token::CANVI_DE_SIGNE) 
            m = operant_CanviSigne(m), s = true;

         else if (m->fe->info.tipus() == token::CANVI_DE_SIGNE and (m->info.tipus() == token::SUMA or m->info.tipus() == token::MULTIPLICACIO)) 
            m = operant_CanviSigne(m), s = true;

         else if(m->info.tipus() >= token::SUMA and m->info.tipus() <= token::RESTA){
            if (m->fe->info.tipus() == token::DIVISIO and m->fd->info.tipus() == token::DIVISIO){
               if (compara_nodes(m->fe->fd, m->fd->fd)) m = factor_comu(m->fe->info, m->info, m->fe->fd, m->fe->fe, m->fd->fe, 0), s= true;
            }
            else if (m->fe->info.tipus() == token::MULTIPLICACIO and m->fd->info.tipus() == token::MULTIPLICACIO){
               if (compara_nodes(m->fe->fd, m->fd->fd)) m = factor_comu(m->fe->info, m->info, m->fe->fd, m->fe->fe, m->fd->fe, 0), s = true;
               else if (compara_nodes(m->fe->fd, m->fd->fe)) m = factor_comu(m->fe->info, m->info, m->fe->fd, m->fe->fe, m->fd->fd, 0), s= true;
               else if (compara_nodes(m->fe->fe, m->fd->fe)) m = factor_comu(m->fe->info, m->info, m->fe->fe, m->fe->fd, m->fd->fd, 0), s= true;
               else if (compara_nodes(m->fe->fe, m->fd->fd)) m = factor_comu(m->fe->info, m->info, m->fe->fe, m->fe->fd, m->fd->fe, 0), s= true;
            }
         }

         else if (m->info.tipus() == token::MULTIPLICACIO or m->info.tipus() == token::DIVISIO){
            if (m->info.tipus() == token::DIVISIO and (m->fd->info.tipus() == token::CT_RACIONAL and m->fd->info.valor_racional().num() == 0)) m->fd->info = token(0);
            else if (m->info.tipus() == token::MULTIPLICACIO and (m->fe->info.tipus() == token::DIVISIO and m->fe->fe->info == token(1))) m = divisio_entre_un(m), s = true;
            else if (m->fe->info.tipus() == token::DIVISIO) m = agrupar_operants(m, m->fe,s); 
            else if (m->fd->info.tipus() == token::DIVISIO and m->fd->fe->info == token(1)) m = divisio_entre_un(m), s = true;
            else if (m->fe->info.tipus() == token::EXPONENCIACIO and m->fd->info.tipus() == token::EXPONENCIACIO){
               if (compara_nodes(m->fe->fd, m->fd->fd)) m = factor_comu(token(token::EXPONENCIACIO), m->info, m->fe->fd, m->fe->fe, m->fd->fe, 0), s = true;
               else if (compara_nodes(m->fe->fe, m->fd->fe)){
                  token tk;
                  if (m->info.tipus() == token::MULTIPLICACIO) tk = token(token::SUMA);
                  else tk = token(token::RESTA);
                  m = factor_comu(token(token::EXPONENCIACIO), tk, m->fe->fe, m->fe->fd, m->fd->fd, 1), s = true;
               } 
            }
            else if (m->fe->info.tipus() == token::EXP and m->fd->info.tipus() == token::EXP){
                  token tk;
                  if (m->info.tipus() == token::MULTIPLICACIO) tk = token(token::SUMA);
                  else tk = token(token::RESTA); 
                  m = factor_comu(token(token::EXP), tk, nullptr, m->fe->fd, m->fd->fd, 0), s = true;
            }
         }

         else if (m->info.tipus() == token::EXPONENCIACIO){
            if (m->fe->info.tipus() < token::VARIABLE and m->fd->info.tipus() == token::CT_ENTERA) m = operacio_simple(m), s = true;
            else if (m->fe->info.tipus() == token::CT_REAL and m->fd->info.tipus() == token::CT_REAL) m = operacio_simple(m), s = true;
            else if (m->fe->info.tipus() == token::EXPONENCIACIO) m = factor_comu(m->info, token(token::MULTIPLICACIO), m->fe->fe, m->fe->fd, m->fd, 1), s = true;
            else if (m->fe->info.tipus() == token::EXP) m = factor_comu(token(token::EXP), token(token::MULTIPLICACIO), nullptr, m->fe->fd, m->fd, 1), s = true;
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
            n->fd = n->fe = nullptr;
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

      else if (m->info.tipus() >= token::SQRT and m->info.tipus() <= token::LOG){
         if (m->info.tipus() == token::SQRT and m->fd->info == token(0)) 
            m = operant_zero(m), s = true;

         else if (m->fd->info.tipus() == token::CT_REAL) 
            m = operacio_simple(m), s = true;

         else if (m->info.tipus() == token::LOG and m->fd->info.tipus() == token::EXP) 
            m = m->fd->fd, s = true;

         else if (m->info.tipus() == token::SQRT and m->fd->info.tipus() <= token::VARIABLE){
               node *n = new node(token(token::EXPONENCIACIO));
               n->fe = m->fd;
               node *n1 = new node(token(racional(1,2)));
               n1->fe = n1->fd = nullptr;
               n->fd = n1;
               m = n;
               s = true;  
         }

         else if (m->info.tipus() == token::EXP and m->fd->info.tipus() == token::LOG){
            m = m->fd->fd;
            s = true;
         }

         else if(m->info.tipus() == token::SQRT and m->fd->info.tipus() == token::CANVI_DE_SIGNE) 
            m  = operant_CanviSigne(m), s = true;

         else if (m->info.tipus() == token::LOG){
            if (m->fd->info.tipus() == token::MULTIPLICACIO or m->fd->info.tipus() == token::DIVISIO or m->fd->info.tipus() == token::EXPONENCIACIO) m = factor_comu_log(m, s);
         }
      }
      else if(m->info.tipus() == token::EVALF){
         token t;
         if (m->fd->info.tipus() < token::VARIABLE){
            if (m->fd->info.tipus() == token::CT_ENTERA) t = token(double(m->fd->info.valor_enter()));
            else if (m->fd->info.tipus() == token::CT_REAL) t = token(m->fd->info.valor_real());
            else if (m->fd->info.tipus() == token::CT_RACIONAL) t = token(double(m->fd->info.valor_racional().num()) / double(m->fd->info.valor_racional().denom()));
            node *n = new node(t);
            m = n;
            s = true;
         }
         else if (m->fd->info.tipus() == token::CT_E){
            t = token(2.718281828);
            node *n = new node(t);
            m = n;
            s = true;
         }
         else if (m->fd->info.tipus() == token::VARIABLE){
            t = token(m->fd->info.identificador_variable());
            node *n = new node(t);
            m = n;
            s = true;
         }
         else if (m->fd->info.tipus() >= token::SUMA and m->fd->info.tipus() <= token::EXPONENCIACIO){
            node *n = new node;
            n->info = m->fd->info;
            node* n1 = new node(token(token::EVALF));
            n1->fd = m->fd->fe;
            node* n2 = new node(token(token::EVALF));
            n2->fd = m->fd->fd;
            n->fe = simplificar(n1, s);
            n->fd = simplificar(n2, s);
            m = n;
            s = true;
         }
         else if (m->fd->info.tipus() >= token::CANVI_DE_SIGNE and m->fd->info.tipus() <= token::LOG){
            node* n = new node(m->fd->info);
            node* n1 = new node(token(token::EVALF));
            n1->fd = m->fd->fd;
            n->fd = simplificar(n1, s);
            m = n;
            s = true;
         }
      }
      return m;
   }

   //Pre: tenim un node m, arbre.
   //Post: Recorrem l'arbre en postordre en bucle, fins que s sigui false. S serà false quan ja no se li pugui aplicar cap simplificació més.
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

   //Pre: Tenim un node m, arbre i una llista de tokens lt.
   //Post: Recorrem l'arbre en inordre i anem afegint els tokens dels nodes a la llista, posant parèntesis on sigui necessari.
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
			else if (m->info.tipus() == token::CANVI_DE_SIGNE){
            lt.insert(lt.end(), m->info);
            if (m->fd->info.tipus() == token::MULTIPLICACIO or m->fd->info.tipus() == token::DIVISIO){
               lt.insert(lt.end(), token(token::OBRIR_PAR));
               llista_tokens(m->fd, lt);
               lt.insert(lt.end(), token(token::TANCAR_PAR));
            }
            else llista_tokens(m->fd, lt);
         }
		   else if (m->info.tipus() <= token::SIGNE_POSITIU){
            if (m->fe->info.tipus() <= token::SIGNE_POSITIU and m->fe->info.tipus() >= token::SUMA and m->info > m->fe->info){
               lt.insert(lt.end(), token(token::OBRIR_PAR));
               llista_tokens(m->fe, lt);
               lt.insert(lt.end(), token(token::TANCAR_PAR));
            }
            else if(m->info.tipus() == token::EXPONENCIACIO and m->fe->info.tipus() == token::EXPONENCIACIO){
               lt.insert(lt.end(), token(token::OBRIR_PAR));
               llista_tokens(m->fe, lt);
               lt.insert(lt.end(), token(token::TANCAR_PAR));
            }

            else llista_tokens(m->fe, lt);
            
            lt.insert(lt.end(), m->info);
            
            if (m->fd->info.tipus() < token::CANVI_DE_SIGNE and m->fd->info.tipus() >= token::SUMA and m->info > m->fd->info){
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
            else if((m->info.tipus() == token::EXPONENCIACIO or m->info.tipus() == token::DIVISIO) and m->fd->info.tipus() == token::CT_RACIONAL){
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
      _arrel = new node(t);
      _arrel->fe = _arrel->fd = nullptr;
      }
   }

  /* Constructora a partir d'una seqüència de tokens. Es produeix un error si
     la seqüència és buida o si no es pot construir l'arbre d'expressió
     corresponent(és a dir, si és sintàcticament incorrecta). */
   expressio::expressio(const list<token> & l) throw(error){
      if (l.size() == 0) throw error(ErrorSintactic);
      list<token>::const_iterator it = l.begin();
      stack<token> op;
      stack<node*> ex;
      while(it != l.end()){
         if (comprova_sintaxis(l, it)){
            if ((*it).tipus() <= token::VARIABLE){
               node *a = new node(*it);
               a->fd = a->fe = nullptr;
               ex.push(a);
            }
            else if ((*it).tipus() <= token::EXPONENCIACIO){
               if (not op.empty() and op.top().tipus() <= token::CANVI_DE_SIGNE){
                  if ((*it) < op.top() or operadors(op.top(), (*it))){
                     while (not op.empty() and op.top().tipus() != token::OBRIR_PAR){
                        node *a = new node(op.top());
                        a->fd = ex.top();
                        ex.pop();
                        if (op.top().tipus() >= token::CANVI_DE_SIGNE and op.top().tipus() <= token::EVALF){
                           a->fe = nullptr;
                        }
                        else{
                           a->fe = ex.top();
                           ex.pop();
                        }
                        ex.push(a);
                        op.pop();
                     } 
                  }
               }
               op.push(*it);
            }
            else if ((*it).tipus() == token::TANCAR_PAR){
               while (op.top().tipus() != token::OBRIR_PAR){
                  if (op.empty()) throw error(ErrorSintactic);
                  else{
                        node *a = new node(op.top());
                        a->fd = ex.top();
                        ex.pop();
                        if (op.top().tipus() >= token::CANVI_DE_SIGNE and op.top().tipus() <= token::EVALF){
                           a->fe = nullptr;
                        }
                        else{
                           a->fe = ex.top();
                           ex.pop();
                        }
                        ex.push(a);
                        op.pop();
                  }
               }
               op.pop();
               if (not op.empty() and (op.top().tipus() >= token::CANVI_DE_SIGNE and op.top().tipus() <= token::EVALF)){
                  node *a = new node(op.top());
                  a->fd = ex.top();
                  a->fe = nullptr;
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
         node *a = new node(op.top());
         a->fd = ex.top();
         ex.pop();
         if (op.top().tipus() >= token::CANVI_DE_SIGNE and op.top().tipus() <= token::EVALF){
            a->fe = nullptr;
         }
         else{
            a->fe = ex.top();
            ex.pop();
         }
         ex.push(a);
         op.pop();
      }
      _arrel = ex.top(); 
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
      return _arrel->info.tipus() == token::NULLTOK; 
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


