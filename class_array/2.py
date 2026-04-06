import matplotlib.pyplot as plt
import csv
import math
import numpy as np

# Чтение данных из CSV
N_values = []
answers = []
times = []

# Укажите ваш путь к файлу
file_path = '/Users/nailed1/Documents/c++/cpp_oop/cpp_oop/LebedevOopCpp/class_array/josephus_results.csv'

try:
    with open(file_path, 'r') as f:
        reader = csv.DictReader(f)
        for row in reader:
            try:
                N_values.append(int(row['N']))
                answers.append(int(row['Answer']))
                times.append(float(row['Time_sec']))
            except (ValueError, KeyError):
                continue
except FileNotFoundError:
    print(f"Файл не найден по пути: {file_path}")
    # Для демонстрации создадим тестовые данные, если файл не найден
    N_values = [10, 100, 1000, 5000, 10000]
    answers = [5, 73, 977, 1809, 3617]
    times = [0.00001, 0.0001, 0.01, 0.25, 1.0]

# Функция для аналитического решения J(N,2)
def josephus_formula(N):
    if N <= 0: return 0
    highest_power = 2 ** int(math.log2(N))
    return 2 * (N - highest_power) + 1

# Расчет вашей теоретической сложности: (N*(N+1))/2 - 1
def theoretical_ops(N):
    return (N * (N + 1)) / 2 - 1

# Создание фигуры
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(16, 6))

# --- ГРАФИК 1: ВРЕМЯ И ВАША ТЕОРЕТИЧЕСКАЯ СЛОЖНОСТЬ ---
ax1.plot(N_values, times, 'bo-', linewidth=2, markersize=6, label='Эксперимент (Array)')

# Масштабируем количество операций в секунды (по последней точке данных)
ops_data = [theoretical_ops(n) for n in N_values]
scale_factor = times[-1] / ops_data[-1]
scaled_ops = [op * scale_factor for op in ops_data]

ax1.plot(N_values, scaled_ops, 'r--', linewidth=2, label=r'Теория: $\frac{N(N+1)}{2} - 1$')

ax1.set_xlabel('N (количество элементов)', fontsize=12)
ax1.set_ylabel('Время (сек)', fontsize=12)
ax1.set_title('Сравнение времени с вашей формулой сложности', fontsize=14)
ax1.legend(fontsize=10)
ax1.grid(True, which="both", ls="-", alpha=0.2)
ax1.set_xscale('log')
ax1.set_yscale('log')

# --- ГРАФИК 2: ПРОВЕРКА ПРАВИЛЬНОСТИ ОТВЕТОВ ---
theoretical_answers = [josephus_formula(n) for n in N_values]

ax2.plot(N_values, theoretical_answers, 'g-', linewidth=3, alpha=0.5, label='Математическая модель $J(N,2)$')
ax2.scatter(N_values, answers, color='red', s=40, edgecolors='black', label='Ваш расчет (Array)', zorder=3)

ax2.set_xlabel('N', fontsize=12)
ax2.set_ylabel('Позиция выжившего', fontsize=12)
ax2.set_title('Совпадение результатов', fontsize=14)
ax2.legend(fontsize=10)
ax2.grid(True, alpha=0.3)
ax2.set_xscale('log')
ax2.set_yscale('log')

plt.tight_layout()
plt.savefig('josephus_full_analysis.png', dpi=150)
plt.show()

# Вывод анализа в консоль
print(f"{'N':<10} | {'Время (с)':<12} | {'Опер. (теор)':<15} | {'Совпадение':<10}")
print("-" * 55)
for i in range(len(N_values)):
    n = N_values[i]
    match = "✓" if answers[i] == theoretical_answers[i] else "✗"
    print(f"{n:<10} | {times[i]:<12.6f} | {int(ops_data[i]):<15} | {match:<10}")

# Экстраполяция на N = 1,000,000
big_n = 10**6
predicted_ops = theoretical_ops(big_n)
predicted_time_sec = predicted_ops * scale_factor
predicted_time_hours = predicted_time_sec / 3600

print("-" * 55)
print(f"ПРОГНОЗ ДЛЯ N = {big_n}:")
print(f"Ваша формула операций: {predicted_ops:,.0f}")
print(f"Ожидаемое время (Array): {predicted_time_hours:.2f} часов")
print(f"Время по формуле J(N,2): < 0.000001 сек")
