#include <stdio.h>
#include <vector>

void merge(std::vector<int>& a, int p, int q, int r) {
    int n1 = q - p + 1; // 왼쪽 부분 배열의 크기
    int n2 = r - q;     // 오른쪽 부분 배열의 크기

    std::vector<int> L(n1 + 1), R(n2 + 1); // 왼쪽과 오른쪽 부분 배열

    for (int i = 0; i < n1; ++i) {
        L[i] = a[p + i]; // A[p..q]를 L[0..n1-1]로 복사
    }
    for (int j = 0; j < n2; ++j) {
        R[j] = a[q + 1 + j]; // A[q+1..r]을 R[0..n2-1]로 복사
    }

    L[n1] = R[n2] = 1'000'000'000; // 무한대 값 (정렬된 배열의 끝을 나타냄)

    int i = 0, j = 0;
    
    for(int k = p; k <= r; ++k) {
        if (L[i] <= R[j]) {
            a[k] = L[i]; // L[i]가 R[j]보다 작거나 같으면 A[k]에 L[i]를 저장
            ++i;
        } else {
            a[k] = R[j]; // 그렇지 않으면 A[k]에 R[j]를 저장
            ++j;
        }
    }
}

// A, p, r이 주어질 때 A[p..r]을 정렬하는 함수
void _internal_merge_sort(std::vector<int>& a, int p, int r) {
    if (p < r)
    {
        int q = (p + r) / 2; // q는 p와 r의 중간 지점
        _internal_merge_sort(a, p, q); // A[p..q]를 정렬
        _internal_merge_sort(a, q + 1, r); // A[q+1..r]을 정렬
        merge(a, p, q, r); // A[p..q]와 A[q+1..r]을 병합하여 A[p..r]을 정렬된 상태로 만듦
    }
}

void _sort_func(std::vector<int>& a) {
    _internal_merge_sort(a, 0, a.size() - 1);
}