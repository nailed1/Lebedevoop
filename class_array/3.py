import matplotlib.pyplot as plt
import csv
import math

# --- 1. ЗАГРУЗКА ДАННЫХ ---
N_values = []
answers = []
times = []
file_path = '/Users/nailed1/Documents/c++/cpp_oop/cpp_oop/LebedevOopCpp/class_array/josephus_results.csv'

try:
    with open(file_path, 'r') as f:
        reader = csv.DictReader(f)
        for row in reader:
            try:
                N_values.append(int(row['N']))
                answers.append(int(row['Answer']))
                times.append(float(row['Time_sec']))
            except (ValueError, KeyError): continue
except FileNotFoundError:
    print("Файл не найден. Используются тестовые значения.")
    N_values = [10, 100, 1000, 5000, 10000]
    times = [0.00001, 0.0005, 0.04, 1.1, 4.5]
    answers = [5, 73, 977, 1809, 3617]

# --- 2. ФУНКЦИИ СЛОЖНОСТИ И МОДЕЛИ ---
def josephus_formula(N):
    if N <= 0: return 0
    return 2 * (N - 2**int(math.log2(N))) + 1

def theory_ops_full(N): return (N * (N + 1)) / 2 - 1  # Ваша формула (худший случай)
def theory_ops_avg(N): return (N**2) / 4            # Усредненная модель

# --- 3. ПОСТРОЕНИЕ ГРАФИКОВ ---
fig, (ax1, ax2, ax3) = plt.subplots(1, 3, figsize=(20, 6))

# ГРАФИК 1: Время и ваша формула (N*(N+1)/2)
ax1.plot(N_values, times, 'bo-', label='Реальное время (сек)')
scale_full = times[-1] / theory_ops_full(N_values[-1])
ax1.plot(N_values, [theory_ops_full(n) * scale_full for n in N_values], 
         'r--', label=r'Теория: $\frac{N(N+1)}{2}-1$')
ax1.set_title('Сравнение с вашей формулой', fontsize=13)
ax1.set_xscale('log'); ax1.set_yscale('log')
ax1.legend(); ax1.grid(True, alpha=0.3)

# ГРАФИК 2: Время и модель N²/4
ax2.plot(N_values, times, 'bo-', label='Реальное время (сек)')
scale_avg = times[-1] / theory_ops_avg(N_values[-1])
ax2.plot(N_values, [theory_ops_avg(n) * scale_avg for n in N_values], 
         'm-.', label=r'Модель: $N^2/4$')
ax2.set_title('Сравнение с моделью $N^2/4$', fontsize=13)
ax2.set_xscale('log'); ax2.set_yscale('log')
ax2.legend(); ax2.grid(True, alpha=0.3)

# ГРАФИК 3: Проверка корректности ответа J(N,2)
theoretical_answers = [josephus_formula(n) for n in N_values]
ax3.plot(N_values, theoretical_answers, 'g-', alpha=0.5, label='Формула $J(N,2)$')
ax3.scatter(N_values, answers, color='red', s=30, label='Ваш Array')
ax3.set_title('Проверка точности результата', fontsize=13)
ax3.set_xscale('log'); ax3.set_yscale('log')
ax3.legend(); ax3.grid(True, alpha=0.3)

plt.tight_layout()
plt.show()

# --- 4. АНАЛИЗ ---
print(f"{'N':<8} | {'Время':<10} | {'(N²+N)/2':<12} | {'N²/4':<12}")
print("-" * 50)
for i in range(len(N_values)):
    n = N_values[i]
    print(f"{n:<8} | {times[i]:<10.5f} | {int(theory_ops_full(n)):<12} | {int(theory_ops_avg(n)):<12}")
