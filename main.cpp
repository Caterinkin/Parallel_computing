#include <iostream>
#include <vector>
#include <thread>
#include <mutex>
#include <chrono>
#include <clocale>

// Функция для вычисления части вектора
void computePartialSum(const std::vector<int>& vec1, const std::vector<int>& vec2,
    std::vector<int>& result, size_t start, size_t end)
{
    for (size_t i = start; i < end; ++i)
    {
        result[i] = vec1[i] + vec2[i];
    }
}

// Функция для параллельного вычисления суммы векторов
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
    // Устанавливаем русскую локаль
    std::setlocale(LC_ALL, "Russian");

    // Выводим количество аппаратных ядер
    unsigned int cores = std::thread::hardware_concurrency();
    std::cout << "Доступное количество аппаратных ядер: " << cores << std::endl;

    // Размеры массивов для тестирования
    std::vector<size_t> sizes = { 1000, 10000, 100000, 1000000 };
    // Количество потоков для тестирования
    std::vector<int> thread_counts = { 2, 4, 8, 16 };

    // Создаем таблицу для результатов
    std::vector<std::vector<double>> results(
        sizes.size(),
        std::vector<double>(thread_counts.size(), 0.0)
    );

    // Проводим тестирование для каждого размера массива
    for (size_t s = 0; s < sizes.size(); ++s)
    {
        size_t size = sizes[s];

        // Создаем векторы
        std::vector<int> vec1(size, 1); // Заполняем единицами для простоты
        std::vector<int> vec2(size, 2); // Заполняем двойками
        std::vector<int> result(size);

        // Тестируем для каждого количества потоков
        for (size_t t = 0; t < thread_counts.size(); ++t)
        {
            int threads = thread_counts[t];

            // Замеряем время выполнения
            auto start = std::chrono::high_resolution_clock::now();

            parallelVectorSum(vec1, vec2, result, threads);

            auto end = std::chrono::high_resolution_clock::now();
            std::chrono::duration<double> duration = end - start;
            results[s][t] = duration.count();

            // Проверка корректности (для первого элемента)
            if (result[0] != 3)
            {
                std::cerr << "Ошибка вычислений!" << std::endl;
                return 1;
            }
        }
    }

    // Выводим таблицу результатов
    std::cout << "\nРезультаты тестирования (время в секундах):\n";
    std::cout << "Размер массива\\Потоки\t";

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