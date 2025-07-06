#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <clocale>

// ������� ��� ���������� ����� �������
void computePartialSum(const std::vector<int>& vec1, const std::vector<int>& vec2,
    std::vector<int>& result, size_t start, size_t end)
{
    for (size_t i = start; i < end; ++i)
    {
        result[i] = vec1[i] + vec2[i];
    }
}

// ������� ��� ������������� ���������� ����� ��������
void parallelVectorSum(const std::vector<int>& vec1, const std::vector<int>& vec2,
    std::vector<int>& result, int num_threads)
{
    std::vector<std::thread> threads;
    size_t chunk_size = vec1.size() / num_threads;

    for (int i = 0; i < num_threads; ++i)
    {
        size_t start = i * chunk_size;
        size_t end = (i == num_threads - 1) ? vec1.size() : start + chunk_size;

        threads.emplace_back(computePartialSum, std::ref(vec1), std::ref(vec2),
            std::ref(result), start, end);
    }

    for (auto& thread : threads)
    {
        thread.join();
    }
}

int main()
{
    // ������������� ������� ������
    std::setlocale(LC_ALL, "Russian");

    // ������� ���������� ���������� ����
    unsigned int cores = std::thread::hardware_concurrency();
    std::cout << "��������� ���������� ���������� ����: " << cores << std::endl;

    // ������� �������� ��� ������������
    std::vector<size_t> sizes = { 1000, 10000, 100000, 1000000 };
    // ���������� ������� ��� ������������
    std::vector<int> thread_counts = { 2, 4, 8, 16 };

    // ������� ������� ��� �����������
    std::vector<std::vector<double>> results(
        sizes.size(),
        std::vector<double>(thread_counts.size(), 0.0)
    );

    // �������� ������������ ��� ������� ������� �������
    for (size_t s = 0; s < sizes.size(); ++s)
    {
        size_t size = sizes[s];

        // ������� �������
        std::vector<int> vec1(size, 1); // ��������� ��������� ��� ��������
        std::vector<int> vec2(size, 2); // ��������� ��������
        std::vector<int> result(size);

        // ��������� ��� ������� ���������� �������
        for (size_t t = 0; t < thread_counts.size(); ++t)
        {
            int threads = thread_counts[t];

            // �������� ����� ����������
            auto start = std::chrono::high_resolution_clock::now();

            parallelVectorSum(vec1, vec2, result, threads);

            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> duration = end - start;
            results[s][t] = duration.count();

            // �������� ������������ (��� ������� ��������)
            if (result[0] != 3)
            {
                std::cerr << "������ ����������!" << std::endl;
                return 1;
            }
        }
    }

    // ������� ������� �����������
    std::cout << "\n���������� ������������ (����� � ��������):\n";
    std::cout << "������ �������\\������\t";

    for (int threads : thread_counts)
    {
        std::cout << threads << "\t";
    }
    std::cout << std::endl;

    for (size_t s = 0; s < sizes.size(); ++s)
    {
        std::cout << sizes[s] << "\t\t\t";

        for (size_t t = 0; t < thread_counts.size(); ++t)
        {
            std::cout << results[s][t] << "\t";
        }
        std::cout << std::endl;
    }

    return 0;
}