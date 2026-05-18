/**
 * Quiz engine for OOP C++ handbook
 */

function initQuiz(containerId) {
  const container = document.getElementById(containerId);
  if (!container) return;

  const checkBtn = container.querySelector('.check-btn');
  const resetBtn = container.querySelector('.reset-btn');
  const scoreEl  = container.querySelector('.score-display');
  const questions = container.querySelectorAll('.question');

  if (checkBtn) checkBtn.addEventListener('click', () => checkQuiz(container, questions, scoreEl));
  if (resetBtn) resetBtn.addEventListener('click', () => resetQuiz(container, questions, scoreEl));
}

function checkQuiz(container, questions, scoreEl) {
  let correct = 0;
  let answered = 0;

  questions.forEach(q => {
    const selected = q.querySelector('input[type=radio]:checked');
    const options = q.querySelectorAll('label');
    const explanation = q.querySelector('.explanation');
    const correctVal = q.dataset.correct;

    // Lock inputs
    q.querySelectorAll('input[type=radio]').forEach(r => r.disabled = true);

    if (!selected) return;
    answered++;

    options.forEach(label => {
      const radio = label.querySelector('input[type=radio]');
      if (!radio) return;
      label.classList.remove('correct', 'wrong');
      if (radio.value === correctVal) {
        label.classList.add('correct');
      } else if (radio.checked && radio.value !== correctVal) {
        label.classList.add('wrong');
      }
    });

    if (selected.value === correctVal) correct++;
    if (explanation) explanation.classList.add('show');
  });

  if (scoreEl) {
    const pct = answered > 0 ? Math.round((correct / questions.length) * 100) : 0;
    let msg = '';
    if (pct === 100) msg = ' — Отлично! Всё верно!';
    else if (pct >= 70) msg = ' — Хорошо, почти!';
    else if (pct >= 50) msg = ' — Неплохо, но стоит повторить.';
    else msg = ' — Стоит ещё раз пройти тему.';
    scoreEl.innerHTML = `Результат: <strong>${correct}/${questions.length}</strong> (${pct}%)${msg}`;
  }
}

function resetQuiz(container, questions, scoreEl) {
  questions.forEach(q => {
    q.querySelectorAll('input[type=radio]').forEach(r => {
      r.checked = false;
      r.disabled = false;
    });
    q.querySelectorAll('label').forEach(l => l.classList.remove('correct', 'wrong'));
    const explanation = q.querySelector('.explanation');
    if (explanation) explanation.classList.remove('show');
  });
  if (scoreEl) scoreEl.innerHTML = '';
}

// Auto-init all quiz blocks on page load
document.addEventListener('DOMContentLoaded', () => {
  document.querySelectorAll('.quiz-block').forEach(block => {
    if (block.id) initQuiz(block.id);
  });

  // Highlight active sidebar link
  const current = window.location.pathname.split('/').pop() || 'index.html';
  document.querySelectorAll('.sidebar-nav a').forEach(a => {
    if (a.getAttribute('href') === current) a.classList.add('active');
  });
});
