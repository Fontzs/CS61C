#include "ll_cycle.h"

#include <stddef.h>

int ll_has_cycle(node *head) {
  node *fast_ptr = head;
  node *slow_ptr = head;
  while (fast_ptr != NULL) {
    node *temp = fast_ptr->next;
    if (temp == NULL || temp->next == NULL) {
      break;
    }
    fast_ptr = temp->next;
    slow_ptr = slow_ptr->next;
    if (fast_ptr == slow_ptr) return 1;
  }
  return 0;
}
