/*
 * Homework 8
 * Zubin Sidhu
 * CS 240, Spring 2025
 * Purdue University
 */

#include "hw8.h"

#include <assert.h>
#include <malloc_debug.h>
#include <stdio.h>
#include <string.h>

/*
 * Duplicate String - Function that takes in a string and creates a copy
 * to allow modification
 */

static char *duplicate_string(const char *s) {
  assert(s != NULL);
  size_t len = strlen(s) + 1;
  char *dup = (char *)malloc(len);
  assert(dup != NULL);
  strcpy(dup, s);
  return dup;
} /* duplicate_string() */

/*
 * Insert Sorted Substance - Inserts substance into a sorted substance_list
 * in non-increasing order. If substances have equal potency, the new substance
 * goes after those already in the list.
 */

static void insert_sorted_substance(substance_t **head, substance_t *new_sub) {
  assert((head != NULL) && (new_sub != NULL));
  if (*head == NULL) {
    new_sub->next = NULL;
    *head = new_sub;
    return;
  }

  substance_t *prev = NULL;
  substance_t *curr = *head;
  while ((curr != NULL) && ((curr->potency > new_sub->potency) ||
         (curr->potency == new_sub->potency))) {
    prev = curr;
    curr = curr->next;
  }
  if (prev == NULL) {
    new_sub->next = *head;
    *head = new_sub;
  }
  else {
    new_sub->next = prev->next;
    prev->next = new_sub;
  }
} /* insert_sorted_substance() */

/*
 * Count Effects - Counts the occurrences of each effect char in a string
 */

static void count_effects(const char *pattern, int counts[3]) {
  counts[0] = 0;
  counts[1] = 0;
  counts[2] = 0;
  for (int i = 0; pattern[i] != '\0'; i++) {
    if (pattern[i] == '@') {
      counts[0]++;
    }
    else if (pattern[i] == '*') {
      counts[1]++;
    }
    else if (pattern[i] == '+') {
      counts[2]++;
    }
  }
} /* count_effects() */

/*
 * Create Substance - Creates a substance structure and populates it with
 * arguments passed. Ensures copy of the name and origin is passed and sets
 * all other fields to NULL. Returns pointer to allocated substance.
 */

substance_t *create_substance(char *substance_name, char *origin,
                              double potency) {
  assert((substance_name != NULL) && (origin != NULL));
  assert(potency >= 0.0);

  substance_t *sub = (substance_t *)malloc(sizeof(substance_t));
  assert(sub != NULL);

  sub->name = duplicate_string(substance_name);
  sub->origin = duplicate_string(origin);
  sub->potency = potency;
  sub->next = NULL;

  return sub;
} /* create_substance() */

/*
 * Create Potion - Creates a potion structure and populates it with
 * arguments passed. Ensures copy of the name and origin is passed and sets
 * all other fields to default. Returns pointer to allocated potion.
 */

potion_t *create_potion(char *potion_name, char *effect_pattern) {
  assert((potion_name != NULL) && (effect_pattern != NULL));

  for (int i = 0; effect_pattern[i] != '\0'; i++) {
    char ch = effect_pattern[i];
    assert((ch == '@') || (ch == '*') || (ch == '+'));
  }

  potion_t *pot = (potion_t *)malloc(sizeof(potion_t));
  assert(pot != NULL);

  pot->name = duplicate_string(potion_name);
  pot->effect_pattern = duplicate_string(effect_pattern);
  pot->total_potency = 0.0;
  pot->substance_list = NULL;
  pot->next = NULL;

  return pot;
} /* create_potion() */

/*
 * Add Substance - Adds a specified substance to the potion with the specified
 * name
 */

int add_substance(potion_t *potions, char *potion_name,
                  substance_t *substance) {
  assert((potions != NULL) && (potion_name != NULL) && (substance != NULL));

  /* Find the potion with the matching name */

  potion_t *pot = potions;
  while (pot != NULL) {
    if (strcmp(pot->name, potion_name) == 0) {
      break;
    }
    pot = pot->next;
  }

  if (pot == NULL) {
    return NOT_FOUND;
  }

  /* Check that a substance with the same name doesn't already exist */

  substance_t *curr = pot->substance_list;
  while (curr != NULL) {
    if (strcmp(curr->name, substance->name) == 0) {
      return INVALID_MOVE;
    }
    curr = curr->next;
  }

  /* Insert substance into the list in sorted order */

  insert_sorted_substance(&(pot->substance_list), substance);
  pot->total_potency += substance->potency;
  return SUCCESS;
} /* add_substance() */

/*
 * Remove Substance - Removes a specified substance from the given potion's
 * substance list
 */

substance_t *remove_substance(potion_t *potion, substance_t *substance) {
  assert((potion != NULL) && (substance != NULL));

  substance_t *prev = NULL;
  substance_t *curr = potion->substance_list;
  while ((curr != NULL) && (curr != substance)) {
    prev = curr;
    curr = curr->next;
  }
  if (curr == NULL) {
    return NULL;
  }
  if (prev == NULL) {
    potion->substance_list = curr->next;
  }
  else {
    prev->next = curr->next;
  }
  curr->next = NULL;
  potion->total_potency -= curr->potency;
  return curr;
} /* remove_substance() */

/*
 * Move Substances - Moves a given substance from one potion to another as well
 * as modifies each potion's potency and substance list pointer.
 */

int move_substances(potion_t *potions, char *from_name, char *to_name,
                    substance_t *substance) {
  assert((potions != NULL) && (from_name != NULL) && (to_name != NULL) &&
         (substance != NULL));

  potion_t *from = NULL;
  potion_t *to = NULL;
  potion_t *curr = potions;
  while (curr != NULL) {
    if (strcmp(curr->name, from_name) == 0) {
      from = curr;
    }
    if (strcmp(curr->name, to_name) == 0) {
      to = curr;
    }
    curr = curr->next;
  }
  if ((from == NULL) || (to == NULL)) {
    return NOT_FOUND;
  }

  /*
   * Check that the destination potion does not already contain a substance
   * with this name
   */

  substance_t *curr_sub = to->substance_list;
  while (curr_sub != NULL) {
    if (strcmp(curr_sub->name, substance->name) == 0) {
      return INVALID_MOVE;
    }
    curr_sub = curr_sub->next;
  }

  substance_t *removed = remove_substance(from, substance);
  if (removed == NULL) {
    return NOT_FOUND;
  }
  insert_sorted_substance(&(to->substance_list), removed);
  to->total_potency += removed->potency;

  return SUCCESS;
} /* move_substances() */

/*
 * Substance Source - Removes all substances from all potions that originate
 * from a given origin, and return a new list sorted in descending order of
 * potency
 */

substance_t *substance_source(potion_t *potions, char *origin) {
  assert((potions != NULL) && (origin != NULL));

  substance_t *result = NULL;
  potion_t *pot = potions;
  while (pot != NULL) {
    substance_t *prev = NULL;
    substance_t *curr = pot->substance_list;
    while (curr != NULL) {
      if (strcmp(curr->origin, origin) == 0) {
        if (prev == NULL) {
          pot->substance_list = curr->next;
        }
        else {
          prev->next = curr->next;
        }
        substance_t *to_move = curr;
        curr = curr->next;

        pot->total_potency -= to_move->potency;
        to_move->next = NULL;
        insert_sorted_substance(&result, to_move);
      }
      else {
        prev = curr;
        curr = curr->next;
      }
    }
    pot = pot->next;
  }
  return result;
} /* substance_source() */

/*
 * Highest Substance Potion - Returns the potion with the most substances
 * In case of a tie, choose the potion with the lowest total_potency and if
 * still tied, the one closer to head
 */

potion_t *highest_substance_potion(potion_t *potions) {
  assert(potions != NULL);

  potion_t *best = NULL;
  potion_t *curr = potions;
  while (curr != NULL) {
    int count = 0;
    substance_t *sub = curr->substance_list;
    while (sub != NULL) {
      count++;
      sub = sub->next;
    }
    if (best == NULL) {
      best = curr;
    }
    else {
      int best_count = 0;
      sub = best->substance_list;
      while (sub != NULL) {
        best_count++;
        sub = sub->next;
      }
      if ((count > best_count) || ((count == best_count) &&
          (curr->total_potency < best->total_potency))) {
        best = curr;
      }
    }
    curr = curr->next;
  }
  return best;
} /* highest_substance_potion() */

/*
 * Remove Potent Substances - Remove and deallocate substances in potions with
 * a matching effect_pattern whose potency exceeds max_potency
 */

int remove_potent_substances(potion_t *potions, char *effect_pattern,
                             double max_potency) {
  assert((potions != NULL) && (effect_pattern != NULL));
  assert(max_potency >= 0.0);

  int found_potion = 0;
  potion_t *pot = potions;
  while (pot != NULL) {
    if (strcmp(pot->effect_pattern, effect_pattern) == 0) {
      substance_t *prev = NULL;
      substance_t *curr = pot->substance_list;
      while (curr != NULL) {
        if (curr->potency > max_potency) {
          if (prev == NULL) {
            pot->substance_list = curr->next;
          }
          else {
            prev->next = curr->next;
          }

          substance_t *temp = curr;
          curr = curr->next;

          pot->total_potency -= temp->potency;

          free(temp->name);
          free(temp->origin);
          free(temp);
          found_potion = 1;
        }
        else {
          prev = curr;
          curr = curr->next;
        }
      }
    }
    pot = pot->next;
  }
  return found_potion ? SUCCESS : NOT_FOUND;
} /* remove_potent_substances() */

/*
 * Purify Potions - Remove and deallocate potions whose dominant effect matches
 * a given effect. The dominant effect is the effect character that appears
 * most often in the potion's effect_pattern. Returns the number of potions
 * removed
 */

int purify_potions(potion_t **potions, char effect) {
  assert((potions != NULL) && (*potions != NULL));
  assert((effect == '@') || (effect == '*') || (effect == '+'));

  int removed_count = 0;
  int found_potion = 0;
  potion_t *curr = *potions;
  potion_t *prev = NULL;

  while (curr != NULL) {
    int counts[3] = {0};
    count_effects(curr->effect_pattern, counts);
    char dominant = '@';
    if ((counts[1] > counts[0]) && (counts[1] >= counts[2])) {
      dominant = '*';
    }
    else if ((counts[2] > counts[0]) && (counts[2] > counts[1])) {
      dominant = '+';
    }

    if (dominant == effect) {
      potion_t *to_remove = curr;
      if (prev == NULL) {
        *potions = curr->next;
      }
      else {
        prev->next = curr->next;
      }
      curr = curr->next;

      substance_t *sub = to_remove->substance_list;
      while (sub != NULL) {
        substance_t *temp = sub;
        sub = sub->next;
        free(temp->name);
        free(temp->origin);
        free(temp);
      }
      free(to_remove->name);
      free(to_remove->effect_pattern);
      free(to_remove);
      removed_count++;
      found_potion = 1;
    }
    else {
      prev = curr;
      curr = curr->next;
    }
  }
  return found_potion ? removed_count : NOT_FOUND;
} /* purify_potions() */

/*
 * Cursed Potions - Remove and deallocate any potion that contains a substance
 * from a cursed origin. Return the number of potions removed.
 */

int cursed_potions(potion_t **potions, char *cursed_origin) {
  assert((potions != NULL) && (*potions != NULL) && (cursed_origin != NULL));

  int removed_count = 0;
  int found_potion = 0;
  potion_t *curr = *potions;
  potion_t *prev = NULL;

  while (curr != NULL) {
    int cursed_found = 0;
    substance_t *sub = curr->substance_list;
    while (sub != NULL) {
      if (strcmp(sub->origin, cursed_origin) == 0) {
        cursed_found = 1;
        break;
      }
      sub = sub->next;
    }
    if (cursed_found) {
      potion_t *to_remove = curr;
      if (prev == NULL) {
        *potions = curr->next;
      }
      else {
        prev->next = curr->next;
      }
      curr = curr->next;

      sub = to_remove->substance_list;
      while (sub != NULL) {
        substance_t *temp = sub;
        sub = sub->next;
        free(temp->name);
        free(temp->origin);
        free(temp);
      }
      free(to_remove->name);
      free(to_remove->effect_pattern);
      free(to_remove);
      removed_count++;
      found_potion = 1;
    }
    else {
      prev = curr;
      curr = curr->next;
    }
  }
  return found_potion ? removed_count : NOT_FOUND;
} /* cursed_potions() */
