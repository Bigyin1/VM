#include "stack/stack.hpp"


int main() {

    stack_s *st = NULL;

    stackError err = E_OK;
    err = stackInit(&st, 0, NULL);
    if (err != E_OK) {
        printf("error\n");

    }

    for (size_t i = 0; i < 33; i++)
    {
        err = stackPush(st, i);
        if (err != E_OK){
            printf("\nerror %d\n", err);
            stackFree(st);
            return 0;
        }
    }

    for (size_t i = 0; i < 33; i++)
    {
        elem_t el = 0;
        err = stackPop(st, &el);
        if (err != E_OK){
            printf("\nerror %d\n", err);
            stackFree(st);
            return 0;
        }
        printf("%lf\n", (double)el);
    }

    stackDump(st, NULL);

    stackFree(st);

}
