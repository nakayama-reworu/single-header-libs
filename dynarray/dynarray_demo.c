#include "dynarray.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>


typedef enum {
    CONST,
    BIN_OP,
    UN_OP
} ItemType;


typedef struct {
    ItemType Type;
    union {
        char Op;
        int Value;
    };
} Item;


Item ItemConst(int value) {
    return (Item) {.Type = CONST, .Value = value};
}


Item ItemUnOp(char op) {
    return (Item) {.Type = UN_OP, .Op = op};
}


Item ItemBinOp(char op) {
    return (Item) {.Type = BIN_OP, .Op = op};
}


int EvalBinOp(char op, int arg1, int arg2) {
    switch (op) {
        case '+':
            return arg1 + arg2;
        case '-':
            return arg1 - arg2;
        case '*':
            return arg1 * arg2;
        case '/':
            return arg1 / arg2;
        default: {
            LOG_ERROR("Unsupported operation: %c", op);
            return 0;
        }
    }
}


int EvalUnOp(char op, int arg1) {
    if ('-' == op) {
        return -arg1;
    }

    LOG_ERROR("Unsupported operation: %c", op);
    return 0;
}


int RpnEval(Item *expression) {
    if (ArrayIsEmpty(expression)) {
        return 0;
    }

    int *operands = ARRAY_EMPTY(int);
    for (typeof(expression) cur = expression; cur != ARRAY_END(expression); cur++) {
        switch (cur->Type) {
            case CONST:
                ARRAY_APPEND(operands, cur->Value);
                break;
            case BIN_OP: {
//                assert(ArraySize(operands) == 2);
                int op1, op2;
                ArrayPop(operands, &op2);
                ArrayPop(operands, &op1);

                ARRAY_APPEND(operands, EvalBinOp(cur->Op, op1, op2));

                break;
            }
            case UN_OP: {
//                assert(ArraySize(operands) == 1);
                int op1;
                ArrayPop(operands, &op1);

                ARRAY_APPEND(operands, EvalUnOp(cur->Op, op1));

                break;
            }
        }
    }

    assert(ArraySize(operands) == 1);
    const int result = operands[0];

    ARRAY_FREE(operands);

    return result;
}


void ItemArrayPrint(Item *items, char *end) {
    if (NULL == end) {
        end = "\n";
    }

    printf(NAME_OF(items)"=[");
    for (typeof(items) item = items; item != ARRAY_END(items); item++) {
        switch (item->Type) {
            case CONST:
                printf("%d%s", item->Value, item + 1 == ARRAY_END(items) ? "" : " ");
                break;
            case BIN_OP:
            case UN_OP:
                printf("%c%s", item->Op, item + 1 == ARRAY_END(items) ? "" : " ");
                break;
        }
    }

    printf("]");
    printf("%s", end);
}


int main() {
    // Create an empty array
    Item *expression = ARRAY_EMPTY(Item);
    ItemArrayPrint(expression, "\n");

    // Append single value -> [1]
    ARRAY_APPEND(expression, ItemConst(1));
    ItemArrayPrint(expression, "\n");

    // Extend with several values -> [1 2 +]
    ARRAY_EXTEND_WITH(expression, ItemConst(2), ItemBinOp('+'));
    ItemArrayPrint(expression, "\n");

    // Temporary array with more items
    Item *minus_two = ARRAY_OF(ItemConst(2), ItemUnOp('-'));

    // Extend the main array with another array -> [1 2 + 2 -]
    expression = ArrayExtend(expression, minus_two);
    ItemArrayPrint(expression, "\n");
    ARRAY_FREE(minus_two);

    // Extend with array literal -> [1 2 + 2 - *]
    ARRAY_EXTEND_WITH_ARRAY_LITERAL(expression, ((Item[]) {ItemBinOp('*')}));
    ItemArrayPrint(expression, "\n");

    // Temporary array with more items
    Item *plus_one = ARRAY_OF_TYPE(Item, 2);
    plus_one[0] = ItemConst(1);
    plus_one[1] = ItemBinOp('+');

    // Extend the main array with another array -> [1 2 + 2 - * 1 +]
    expression = ArrayExtend(expression, plus_one);
    ItemArrayPrint(expression, "\n");
    ARRAY_FREE(plus_one);

    ItemArrayPrint(expression, " -> ");
    printf("%d\n", RpnEval(expression));

    // Pop the last "+1" operation -> [1 2 + 2 - *]
    ArrayPop(expression, NULL);
    ArrayPop(expression, NULL);

    ItemArrayPrint(expression, " -> ");
    printf("%d\n", RpnEval(expression));

    ARRAY_FREE(expression);

    return EXIT_SUCCESS;
}
