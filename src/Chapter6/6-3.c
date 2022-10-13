#include <stdio.h>

int game_nonrecursive(int a, int b, int yn)
{
    char info;
    for (int i = 0; i < yn && a != b; i++)
    {
        info = getchar();
        
        int mid = (a + b) / 2;
        if (info == 'Y') a = mid + 1;
        else b = mid;
    }

    return a;
}

int game_recursive(int a, int b)
{
    if (a == b) return a;

    char info = getchar();
    
    int mid = (a + b) / 2;
    if (info == 'Y') return game_recursive(mid + 1, b);
    else return game_recursive(a, mid);
}

int main(void)
{
    int a, b, yn;
    char info;

    scanf("%d %d %d", &a, &b, &yn);
    getchar();
    printf("%d\n", game_recursive(a, b));

    return 0;
}