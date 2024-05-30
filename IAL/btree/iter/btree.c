/*
 * Binárny vyhľadávací strom — iteratívna varianta
 *
 * S využitím dátových typov zo súboru btree.h, zásobníkov zo súborov stack.h a
 * stack.c a pripravených kostier funkcií implementujte binárny vyhľadávací
 * strom bez použitia rekurzie.
 */

#include "../btree.h"
#include "stack.h"
#include <stdio.h>
#include <stdlib.h>

/*
 * Inicializácia stromu.
 *
 * Užívateľ musí zaistiť, že incializácia sa nebude opakovane volať nad
 * inicializovaným stromom. V opačnom prípade môže dôjsť k úniku pamäte (memory
 * leak). Keďže neinicializovaný ukazovateľ má nedefinovanú hodnotu, nie je
 * možné toto detegovať vo funkcii.
 */
void bst_init(bst_node_t **tree)
{
  (*tree) = NULL;
}

/*
 * Nájdenie uzlu v strome.
 *
 * V prípade úspechu vráti funkcia hodnotu true a do premennej value zapíše
 * hodnotu daného uzlu. V opačnom prípade funckia vráti hodnotu false a premenná
 * value ostáva nezmenená.
 *
 * Funkciu implementujte iteratívne bez použitia vlastných pomocných funkcií.
 */
bool bst_search(bst_node_t *tree, char key, int *value)
{
  bool finish = false, found = false;
  while (!finish)
  {
    if (tree != NULL)
    {
      if (key < tree->key)
      { // hladany uzol ma mensi kluc ako momentalny
        tree = tree->left;
      }
      else if (key > tree->key)
      { // hladany uzol ma vacsi kluc ako momentalny
        tree = tree->right;
      }
      else
      { // hladany uzol sme uspesne nasli
        finish = found = true;
        (*value) = tree->value;
      }
    }
    else
    { // dorazili sme do prazdneho uzlu, cize vyhladavanie je neuspesne
      finish = true;
    }
  }
  return found;
}

/*
 * Vloženie uzlu do stromu.
 *
 * Pokiaľ uzol so zadaným kľúčom v strome už existuje, nahraďte jeho hodnotu.
 * Inak vložte nový listový uzol.
 *
 * Výsledný strom musí spĺňať podmienku vyhľadávacieho stromu — ľavý podstrom
 * uzlu obsahuje iba menšie kľúče, pravý väčšie.
 *
 * Funkciu implementujte iteratívne bez použitia vlastných pomocných funkcií.
 */
void bst_insert(bst_node_t **tree, char key, int value)
{
  bst_node_t *tmp = *tree;

  bst_node_t *new = malloc(sizeof(bst_node_t));
  if (new == NULL)
  {
    return;
  }

  new->key = key;
  new->value = value;
  new->left = NULL;
  new->right = NULL;

  if (tmp == NULL)
  { // strom je uplne prazdny
    *tree = new;
    return;
  }

  while (tmp != NULL)
  {
    if (key > tmp->key)
    { // hladany kluc je vacsi ako kluc momentalneho uzlu
      if (tmp->right == NULL)
      {
        tmp->right = new;
        break;
      }
      else
      {
        tmp = tmp->right;
      }
    }
    else if (key < tmp->key)
    { // hladany kluc je mensi ako kluc momentalneho uzlu
      if (tmp->left == NULL)
      {
        tmp->left = new;
        break;
      }
      else
      {
        tmp = tmp->left;
      }
    }
    else // hladany kluc obsahuje uz existujuci uzol
    {
      tmp->value = value;
      free(new);
      break;
    }
  }
}

/*
 * Pomocná funkcia ktorá nahradí uzol najpravejším potomkom.
 *
 * Kľúč a hodnota uzlu target budú nahradené kľúčom a hodnotou najpravejšieho
 * uzlu podstromu tree. Najpravejší potomok bude odstránený. Funkcia korektne
 * uvoľní všetky alokované zdroje odstráneného uzlu.
 *
 * Funkcia predpokladá že hodnota tree nie je NULL.
 *
 * Táto pomocná funkcia bude využitá pri implementácii funkcie bst_delete.
 *
 * Funkciu implementujte iteratívne bez použitia vlastných pomocných funkcií.
 */
void bst_replace_by_rightmost(bst_node_t *target, bst_node_t **tree)
{
  bst_node_t *tmp = *tree;
  bst_node_t *prev = target;

  if (tmp == NULL)
  { // strom je prazdny, funkcia bst_replace_by_rightmost() nemoze byt vykonana
    return;
  }

  while (tmp->right != NULL)
  {
    prev = tmp;
    tmp = tmp->right;
  }

  if (tmp->left == NULL)
  {
    target->value = tmp->value;
    target->key = tmp->key;
    if (target != prev)
    {
      prev->right = NULL;
    }
    else
    {
      prev->left = NULL;
    }
    free(tmp);
  }
  else
  {
    target->value = tmp->value;
    target->key = tmp->key;

    prev->right = tmp->left;
    free(tmp);
  }
}

/*
 * Odstránenie uzlu v strome.
 *
 * Pokiaľ uzol so zadaným kľúčom neexistuje, funkcia nič nerobí.
 * Pokiaľ má odstránený uzol jeden podstrom, zdedí ho otec odstráneného uzla.
 * Pokiaľ má odstránený uzol oba podstromy, je nahradený najpravejším uzlom
 * ľavého podstromu. Najpravejší uzol nemusí byť listom!
 * Funkcia korektne uvoľní všetky alokované zdroje odstráneného uzlu.
 *
 * Funkciu implementujte iteratívne pomocou bst_replace_by_rightmost a bez
 * použitia vlastných pomocných funkcií.
 */
void bst_delete(bst_node_t **tree, char key)
{
  bst_node_t *tmp = *tree;
  bst_node_t *prev = NULL;

  while (tmp != NULL)
  {
    if (tmp->key == key)
    { // nasli sme hladany uzol
      if (tmp->left == NULL || tmp->right == NULL)
      { // uzol obsahoval najviac jedno dieta
        if (tmp->left == NULL && tmp->right == NULL)
        { // listovy uzol, ziadne deti
          if (tmp == *tree)
          { // hladany uzol je koren celeho stromu
            free(tmp);
            return;
          }

          if (prev->left == tmp)
          { // hladany uzol bol lavy podstrom
            prev->left = NULL;
          }
          if (prev->right == tmp) // hladany uzol bol pravy podstrom jeho otcovskeho
          {                       // hladany uzol bol pravy podstrom
            prev->right = NULL;
          }

          free(tmp);
          break;
        }
        else
        { // hladany uzol obsahuje 1 dieta
          if (prev != NULL)
          { // odstranovany uzol nie je koren celeho stromu, pretoze existuje uzol ktory na neho ukazuje
            if (prev->left == tmp)
            { // odstranovany uzol je lavy podstrom
              if (tmp->left != NULL)
              { // odstranovany uzol obsahuje len lave dieta
                prev->left = tmp->left;
              }
              else if (tmp->right != NULL)
              { // odstranovany uzol obsahuje len prave dieta
                prev->left = tmp->right;
              }
            }
            else if (prev->right == tmp)
            { // odstranovany uzol je pravy podstrom
              if (tmp->left != NULL)
              { // odstranovany uzol obsahuje len lave dieta
                prev->right = tmp->left;
              }
              else if (tmp->right != NULL)
              { // odstranovany uzol obsahuje len prave dieta
                prev->right = tmp->right;
              }
            }

            free(tmp);
            break;
          }
          else
          { // tmp je koren celeho stromu
            if (tmp->left != NULL)
            {
              *tree = tmp->left;
            }
            else if (tmp->right != NULL)
            {
              *tree = tmp->right;
            }

            free(tmp);
            break;
          }
        }
      }
      else // koren ma 2 deti
      {
        bst_replace_by_rightmost(tmp, &(tmp->left));
      }
    }

    // v tomto cykle sme hladany uzol nenasli
    if (key > tmp->key) // hladanie pokracuje v pravom podstrome
    {
      if (tmp->right != NULL)
      {
        prev = tmp;
        tmp = tmp->right;
      }
      else
      {
        break;
      }
    }
    else // hladanie pokracuje v lavom podstrome
    {
      if (tmp->left != NULL)
      {
        prev = tmp;
        tmp = tmp->left;
      }
      else
      {
        break;
      }
    }
  }
}

/*
 * Zrušenie celého stromu.
 *
 * Po zrušení sa celý strom bude nachádzať v rovnakom stave ako po
 * inicializácii. Funkcia korektne uvoľní všetky alokované zdroje rušených
 * uzlov.
 *
 * Funkciu implementujte iteratívne pomocou zásobníku uzlov a bez použitia
 * vlastných pomocných funkcií.
 */
void bst_dispose(bst_node_t **tree)
{
  if ((*tree) != NULL)
  {
    stack_bst_t stack;
    stack_bst_init(&stack);

    stack_bst_push(&stack, *tree);
    while (!stack_bst_empty(&stack))
    {
      bst_node_t *tmp = stack_bst_pop(&stack);
      if (tmp != NULL)
      {
        stack_bst_push(&stack, tmp->left);
        stack_bst_push(&stack, tmp->right);

        free(tmp);
      }
      else
      {
        continue;
      }
    }
    *tree = NULL;
  }
}

/*
 * Pomocná funkcia pre iteratívny preorder.
 *
 * Prechádza po ľavej vetve k najľavejšiemu uzlu podstromu.
 * Nad spracovanými uzlami zavola bst_print_node a uloží ich do zásobníku uzlov.
 *
 * Funkciu implementujte iteratívne pomocou zásobníku uzlov a bez použitia
 * vlastných pomocných funkcií.
 */
void bst_leftmost_preorder(bst_node_t *tree, stack_bst_t *to_visit)
{
  bst_node_t *tmp = tree;

  while (tmp != NULL)
  {
    stack_bst_push(to_visit, tmp);
    bst_print_node(tmp);
    tmp = tmp->left;
  }
}

/*
 * Preorder prechod stromom.
 *
 * Pre aktuálne spracovávaný uzol nad ním zavolajte funkciu bst_print_node.
 *
 * Funkciu implementujte iteratívne pomocou funkcie bst_leftmost_preorder a
 * zásobníku uzlov bez použitia vlastných pomocných funkcií.
 */
void bst_preorder(bst_node_t *tree)
{
  bst_node_t *tmp = tree;

  stack_bst_t stack;
  stack_bst_init(&stack);

  bst_leftmost_preorder(tmp, &stack);

  while (!stack_bst_empty(&stack))
  {
    tmp = stack_bst_top(&stack);
    stack_bst_pop(&stack);
    bst_leftmost_preorder(tmp->right, &stack);
  }
}

/*
 * Pomocná funkcia pre iteratívny inorder.
 *
 * Prechádza po ľavej vetve k najľavejšiemu uzlu podstromu a ukladá uzly do
 * zásobníku uzlov.
 *
 * Funkciu implementujte iteratívne pomocou zásobníku uzlov a bez použitia
 * vlastných pomocných funkcií.
 */
void bst_leftmost_inorder(bst_node_t *tree, stack_bst_t *to_visit)
{
  bst_node_t *tmp = tree;

  while (tmp != NULL)
  {
    stack_bst_push(to_visit, tmp);
    tmp = tmp->left;
  }
}

/*
 * Inorder prechod stromom.
 *
 * Pre aktuálne spracovávaný uzol nad ním zavolajte funkciu bst_print_node.
 *
 * Funkciu implementujte iteratívne pomocou funkcie bst_leftmost_inorder a
 * zásobníku uzlov bez použitia vlastných pomocných funkcií.
 */
void bst_inorder(bst_node_t *tree)
{
  bst_node_t *tmp = tree;

  stack_bst_t stack;
  stack_bst_init(&stack);

  bst_leftmost_inorder(tmp, &stack);

  while (!stack_bst_empty(&stack))
  {
    tmp = stack_bst_top(&stack);
    stack_bst_pop(&stack);
    bst_print_node(tmp);
    bst_leftmost_inorder(tmp->right, &stack);
  }
}

/*
 * Pomocná funkcia pre iteratívny postorder.
 *
 * Prechádza po ľavej vetve k najľavejšiemu uzlu podstromu a ukladá uzly do
 * zásobníku uzlov. Do zásobníku bool hodnôt ukladá informáciu že uzol
 * bol navštívený prvý krát.
 *
 * Funkciu implementujte iteratívne pomocou zásobníkov uzlov a bool hodnôt a bez použitia
 * vlastných pomocných funkcií.
 */
void bst_leftmost_postorder(bst_node_t *tree, stack_bst_t *to_visit,
                            stack_bool_t *first_visit)
{
  bst_node_t *tmp = tree;

  while (tmp != NULL)
  {
    stack_bst_push(to_visit, tmp);
    stack_bool_push(first_visit, true);
    tmp = tmp->left;
  }
}

/*
 * Postorder prechod stromom.
 *
 * Pre aktuálne spracovávaný uzol nad ním zavolajte funkciu bst_print_node.
 *
 * Funkciu implementujte iteratívne pomocou funkcie bst_leftmost_postorder a
 * zásobníkov uzlov a bool hodnôt bez použitia vlastných pomocných funkcií.
 */
void bst_postorder(bst_node_t *tree)
{
  bst_node_t *tmp = tree;
  bool fromLeft;

  stack_bst_t stack;
  stack_bst_init(&stack);

  stack_bool_t stackBool;
  stack_bool_init(&stackBool);

  bst_leftmost_postorder(tmp, &stack, &stackBool);

  while (!stack_bst_empty(&stack))
  {
    tmp = stack_bst_top(&stack);
    stack_bst_pop(&stack);
    fromLeft = stack_bool_pop(&stackBool);
    if (fromLeft)
    {
      stack_bst_push(&stack, tmp);
      stack_bool_push(&stackBool, false);
      bst_leftmost_postorder(tmp->right, &stack, &stackBool);
    }
    else
    {
      bst_print_node(tmp);
    }
  }
}
