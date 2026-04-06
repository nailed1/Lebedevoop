import matplotlib.pyplot as plt
import csv
import math

# Чтение данных из CSV
N_values = []
answers = []
times = []

with open('/Users/nailed1/Documents/c++/cpp_oop/cpp_oop/LebedevOopCpp/class_array/josephus_results.csv', 'r') as f:
    reader = csv.DictReader(f)
    for row in reader:
        # Пропускаем строки с некорректными данными (комментарии и т.п.)
        try:
            N_values.append(int(row['N']))
            answers.append(int(row['Answer']))
            times.append(float(row['Time_sec']))
        except (ValueError, KeyError):
            continue

# Создание фигуры с подграфиками
fig, (ax1, ax2) = plt.subplots(1, 2, figsize=(14, 5))

# График 1: Зависимость времени выполнения от N
ax1.plot(N_values, times, 'bo-', linewidth=2, markersize=8, label='Эксперимент')

# Теоретическая кривая O(N²)
N_theory = [N_values[0], N_values[-1]]
coeff = times[0] / (N_values[0] ** 2)
theory_quadratic = [coeff * n ** 2 for n in N_theory]
ax1.plot(N_theory, theory_quadratic, 'r--', linewidth=2, label='O(N²) теоретическая')

ax1.set_xlabel('N (количество элементов)', fontsize=12)
ax1.set_ylabel('Время (сек)', fontsize=12)
ax1.set_title('Задача Иосифа Флавия: Время выполнения\n(реализация на Array)', fontsize=14)
ax1.legend(fontsize=10)
ax1.grid(True, alpha=0.3)
ax1.set_xscale('log')
ax1.set_yscale('log')

# График 2: Проверка правильности ответа
# Для k=2 существует формула: J(N,2) = 2*(N - 2^floor(log2(N))) + 1
def josephus_formula(N, k=2):
    if k == 2:
        highest_power = 2 ** int(math.log2(N))
        return 2 * (N - highest_power) + 1
    return None

theoretical_answers = [josephus_formula(n) for n in N_values]

ax2.plot(N_values, theoretical_answers, 'go-', linewidth=2, markersize=8, label='Теоретическая формула')
ax2.plot(N_values, answers, 'rs--', linewidth=2, markersize=8, label='Наш расчёт (Array)')

# Линия идеального совпадения
min_val = min(min(theoretical_answers), min(answers))
max_val = max(max(theoretical_answers), max(answers))
ax2.plot([min_val, max_val], [min_val, max_val], 'k:', alpha=0.5, label='Идеальное совпадение')

ax2.set_xlabel('N (количество элементов)', fontsize=12)
ax2.set_ylabel('Ответ (позиция выжившего)', fontsize=12)
ax2.set_title('Задача Иосифа Флавия: Проверка правильности\n(k=2)', fontsize=14)
ax2.legend(fontsize=10)
ax2.grid(True, alpha=0.3)
ax2.set_xscale('log')
ax2.set_yscale('log')

plt.tight_layout()
plt.savefig('josephus_plot.png', dpi=150, bbox_inches='tight')
plt.show()

# Вывод статистики
print("Проверка правильности расчётов:")
print("-" * 50)
print(f"{'N':<12} {'Расчёт':<10} {'Формула':<10} {'Совпадает':<10}")
print("-" * 50)
for n, ans, theor in zip(N_values, answers, theoretical_answers):
    match = "✓" if ans == theor else "✗"
    print(f"{n:<12} {ans:<10} {theor:<10} {match:<10}")
print("-" * 50)
all_match = all(a == t for a, t in zip(answers, theoretical_answers))
print(f"Все расчёты верны: {'✓ ДА' if all_match else '✗ НЕТ'}")
