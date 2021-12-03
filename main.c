#include <stdbool.h>
#include <stdio.h>

#define MAX_DEPTH_X 501
#define MAX_DEPTH_Y 101
#define MAX_DEPTH_Z 101
#define MAX_DEPTH_STACK (MAX_DEPTH_X * MAX_DEPTH_Y * MAX_DEPTH_Z)

// DFS EXPLORATION STACK

/**
 * A structure which represents a traversal stack, used when building the dynamic programming array. When building the
 * array, we want to use some heuristics in order to improve performance. Especially, a good heuristic is that the
 * items will generally not be consumed, and that the first path to explore should possibly be to try to go up to the
 * last cell of the array without actually buying any good.
 *
 * The rationale for this approach is that, if multiple paths were to lead to the final cell, we'd be better off
 * exploring the paths up to the end rather than necessarily building the whole table.
 */
typedef struct dfs_stack {
  size_t offset;
  int data[3 * MAX_DEPTH_STACK];
} dfs_stack_t;

/**
 * Pushes a new item on the DFS stack. The caller should make sure the stack has enough capacity before pushing !
 *
 * @param stack the stack to which an item is added.
 * @param item the item that is pushed.
 */
inline void dfs_stack_push(dfs_stack_t *stack, int item) {
  stack->data[stack->offset] = item;
  stack->offset++;
}

/**
 * Pops an item from the DFS stack. The caller should make sure that the stack is not empty before popping !
 *
 * @param stack the stack from which the item is popped.
 * @return the item that was just popped.
 */
inline int dfs_stack_pop(dfs_stack_t *stack) {
  stack->offset--;
  return stack->data[stack->offset];
}

// MEMOIZATION DATA STRUCTURES

/**
 * The type of the memoized positions. When arriving on a cell, there are multiple choices : it can be the target cell
 * (in which case we're done solving the problem !), it may be the final
 */
typedef enum memo_item {
  Unknown = 0b0, // The cell has not been visited.
  BadPath = 0b1, // The cell has been visited, but it's a bad path.
} memo_item_t;

/**
 * The recursion stack which we'll be using.
 */
dfs_stack_t stack;

/**
 * The memoization table. Filled up with Unknown when initialized.
 */
memo_item_t table[MAX_DEPTH_X][MAX_DEPTH_Y][MAX_DEPTH_Z];

/**
 * Solves the problem. The algorithm will try to navigate to the cell located at location [c][j][k], and store on a
 * stack the other cells that it might be required to visit afterwards. If, at some point, the cell popped from the
 * stack corresponds to [n][j][k].
 *
 * This algorithm assumes that the memoization table is filled when Unknown values.
 *
 * @param n the number of items
 * @param j the target price
 * @param k the target calories
 * @param prices the prices of the available products
 * @param calories the calories of the available products
 *
 * @return true if there is a solution, false otherwise
 */
bool solve(int n, int j, int k, const int prices[n], const int calories[n]) {

  // Start at the initial cell, located at the bottom left.
  dfs_stack_push(&stack, 0);
  dfs_stack_push(&stack, 0);
  dfs_stack_push(&stack, 0);

  while (stack.offset > 0) {
    int x = dfs_stack_pop(&stack);
    int y = dfs_stack_pop(&stack);
    int z = dfs_stack_pop(&stack);

    if (x == n && y == j && z == k) return true;

    memo_item_t memo = table[x][y][z];
    if (memo == BadPath) continue;

    // We have not visited this part of the tree yet. We must therefore add some cells to the recursion stack.
    int new_x = x + 1;
    if (new_x <= n) {

      // 1. we decide to buy the i-th item.
      int new_y = y + prices[x];
      int new_z = z + calories[x];
      if (new_y <= j && new_z <= k) {
        dfs_stack_push(&stack, new_z);
        dfs_stack_push(&stack, new_y);
        dfs_stack_push(&stack, new_x);
      }

      // 2. we decide not to buy any item
      dfs_stack_push(&stack, z);
      dfs_stack_push(&stack, y);
      dfs_stack_push(&stack, new_x);
    }

    table[x][y][z] = BadPath;
  }
  return false;
}

// ALGORITHM

/**
 * The main entry point of the program.
 */
int main() {
  int n, c, e;
  scanf("%d %d %d", &n, &c, &e);

  int prices[n];
  int calories[n];

  for (int i = 0; i < n; ++i) {
    scanf("%d %d", &prices[i], &calories[i]);
  }

  bool success = solve(n, c, e, prices, calories);
  if (success) {
    printf("Yes\n");
    fflush(stdout);
  } else {
    printf("No\n");
    fflush(stdout);
  }
  return 0;
}
