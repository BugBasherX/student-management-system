/* ═══════════════════════════════════════
   Student Management System — App Logic
   ═══════════════════════════════════════ */

const API = '';
let students        = [];
let sortCol         = 'id';
let sortDir         = 1;
let pendingDeleteId = null;

/* ── Auth ────────────────────────────── */
function isLoggedIn() { return sessionStorage.getItem('sms_auth') === '1'; }

document.getElementById('login-form').addEventListener('submit', async e => {
  e.preventDefault();
  const btn = document.getElementById('login-btn');
  const err = document.getElementById('login-error');
  btn.textContent = 'Signing in…'; btn.disabled = true;
  err.classList.add('hidden');

  try {
    const res  = await fetch(`${API}/api/login`, {
      method: 'POST', headers: {'Content-Type':'application/json'},
      body: JSON.stringify({
        username: document.getElementById('username').value,
        password: document.getElementById('password').value
      })
    });
    const data = await res.json();
    if (data.ok) { sessionStorage.setItem('sms_auth', '1'); showApp(); }
    else err.classList.remove('hidden');
  } catch {
    err.textContent = 'Server unreachable. Try again.';
    err.classList.remove('hidden');
  }
  btn.textContent = 'Sign In'; btn.disabled = false;
});

function logout() {
  sessionStorage.removeItem('sms_auth');
  document.getElementById('app').classList.add('hidden');
  document.getElementById('login-screen').classList.remove('hidden');
  document.getElementById('username').value = '';
  document.getElementById('password').value = '';
}

function showApp() {
  document.getElementById('login-screen').classList.add('hidden');
  document.getElementById('app').classList.remove('hidden');
  showSection('dashboard');
  loadStudents();
}

/* ── Navigation ──────────────────────── */
function showSection(name) {
  document.querySelectorAll('.section').forEach(s => s.classList.add('hidden'));
  document.querySelectorAll('.nav-link').forEach(a => {
    a.classList.toggle('active', a.dataset.section === name);
  });
  document.getElementById(`section-${name}`).classList.remove('hidden');
}

/* ── Data ────────────────────────────── */
async function loadStudents() {
  try {
    const res = await fetch(`${API}/api/students`);
    students  = await res.json();
    renderTable();
    renderDashboard();
  } catch {
    showToast('Failed to load students', 'error');
  }
}

/* ── Dashboard ───────────────────────── */
function renderDashboard() {
  const n = students.length;
  document.getElementById('stat-total').textContent = n;

  if (n > 0) {
    const avg = (students.reduce((s, x) => s + x.gpa, 0) / n).toFixed(2);
    document.getElementById('stat-avg-gpa').textContent = avg;
    const top = Math.max(...students.map(s => s.gpa)).toFixed(2);
    document.getElementById('stat-top-gpa').textContent = top;
  } else {
    document.getElementById('stat-avg-gpa').textContent = '—';
    document.getElementById('stat-top-gpa').textContent = '—';
  }

  document.getElementById('stat-courses').textContent =
    new Set(students.map(s => s.course)).size;

  /* Recent 5 */
  const recent = [...students].sort((a, b) => b.id - a.id).slice(0, 5);
  const rBody  = document.getElementById('recent-body');
  if (recent.length === 0) {
    rBody.innerHTML = `<tr><td colspan="3" class="empty-row">No students yet</td></tr>`;
  } else {
    rBody.innerHTML = recent.map(s => `
      <tr>
        <td>${esc(s.name)}</td>
        <td style="color:var(--t2)">${esc(s.course)}</td>
        <td>${gpaChip(s.gpa)}</td>
      </tr>`).join('');
  }

  /* Top performer */
  const perf = document.getElementById('top-performer');
  if (n === 0) {
    perf.innerHTML = `<div class="performer-empty">No students yet</div>`;
  } else {
    const star = students.reduce((best, s) => s.gpa > best.gpa ? s : best);
    perf.innerHTML = `
      <div class="performer-body">
        <div class="performer-avatar">${star.name[0].toUpperCase()}</div>
        <div class="performer-name">${esc(star.name)}</div>
        <div class="performer-course">${esc(star.course)}</div>
        <div class="performer-gpa">${star.gpa.toFixed(2)}</div>
        <div class="performer-gpa-label">GPA</div>
      </div>`;
  }
}

/* ── Student Table ───────────────────── */
function renderTable() {
  const q    = document.getElementById('search-input').value.toLowerCase().trim();
  let list   = students.filter(s =>
    !q ||
    s.name.toLowerCase().includes(q) ||
    s.course.toLowerCase().includes(q) ||
    String(s.id).includes(q)
  );

  list.sort((a, b) => {
    let av = a[sortCol], bv = b[sortCol];
    if (typeof av === 'string') av = av.toLowerCase();
    if (typeof bv === 'string') bv = bv.toLowerCase();
    return (av < bv ? -1 : av > bv ? 1 : 0) * sortDir;
  });

  const tbody = document.getElementById('student-body');
  const empty = document.getElementById('empty-state');
  const count = document.getElementById('student-count');
  count.textContent = `${list.length} student${list.length !== 1 ? 's' : ''}`;

  if (list.length === 0) {
    tbody.innerHTML = '';
    empty.classList.remove('hidden');
    return;
  }
  empty.classList.add('hidden');

  tbody.innerHTML = list.map(s => `
    <tr>
      <td><span class="id-chip">#${s.id}</span></td>
      <td><strong>${esc(s.name)}</strong></td>
      <td style="color:var(--t2)">${s.age}</td>
      <td style="color:var(--t2)">${esc(s.course)}</td>
      <td>${gpaChip(s.gpa)}</td>
      <td>
        <div class="action-row">
          <button class="icon-btn edit-btn" onclick="openModal(${s.id})" title="Edit">
            <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
              <path d="M11 4H4a2 2 0 0 0-2 2v14a2 2 0 0 0 2 2h14a2 2 0 0 0 2-2v-7"/>
              <path d="M18.5 2.5a2.121 2.121 0 0 1 3 3L12 15l-4 1 1-4 9.5-9.5z"/>
            </svg>
          </button>
          <button class="icon-btn del-btn" onclick="confirmDelete(${s.id},'${esc(s.name)}')" title="Delete">
            <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
              <polyline points="3 6 5 6 21 6"/>
              <path d="M19 6l-1 14a2 2 0 0 1-2 2H8a2 2 0 0 1-2-2L5 6"/>
              <path d="M10 11v6M14 11v6M9 6V4h6v2"/>
            </svg>
          </button>
        </div>
      </td>
    </tr>`).join('');
}

function sortBy(col) {
  sortDir = (sortCol === col) ? sortDir * -1 : 1;
  sortCol = col;
  document.querySelectorAll('.si').forEach(el => { el.textContent = '↕'; el.classList.remove('on'); });
  const icon = document.querySelector(`.si[data-col="${col}"]`);
  if (icon) { icon.textContent = sortDir === 1 ? '↑' : '↓'; icon.classList.add('on'); }
  renderTable();
}

/* ── Add / Edit Modal ────────────────── */
function openModal(id = null) {
  const form = document.getElementById('student-form');
  form.reset();
  document.getElementById('form-error').classList.add('hidden');

  if (id) {
    const s = students.find(x => x.id === id);
    if (!s) return;
    document.getElementById('edit-id').value  = s.id;
    document.getElementById('f-name').value   = s.name;
    document.getElementById('f-age').value    = s.age;
    document.getElementById('f-course').value = s.course;
    document.getElementById('f-gpa').value    = s.gpa;
    document.getElementById('modal-title').textContent = 'Edit Student';
    document.getElementById('form-btn').textContent    = 'Save Changes';
  } else {
    document.getElementById('edit-id').value = '';
    document.getElementById('modal-title').textContent = 'Add Student';
    document.getElementById('form-btn').textContent    = 'Add Student';
  }
  document.getElementById('modal-overlay').classList.remove('hidden');
  setTimeout(() => document.getElementById('f-name').focus(), 60);
}

function closeModal()    { document.getElementById('modal-overlay').classList.add('hidden'); }
function closeModalOut(e){ if (e.target === document.getElementById('modal-overlay')) closeModal(); }

async function submitStudent(e) {
  e.preventDefault();
  const errEl  = document.getElementById('form-error');
  errEl.classList.add('hidden');

  const id     = document.getElementById('edit-id').value;
  const name   = document.getElementById('f-name').value.trim();
  const age    = parseInt(document.getElementById('f-age').value);
  const course = document.getElementById('f-course').value.trim();
  const gpa    = parseFloat(document.getElementById('f-gpa').value);

  if (!name)                    { showFErr('Name is required.'); return; }
  if (isNaN(age) || age < 1)   { showFErr('Enter a valid age.'); return; }
  if (isNaN(gpa) || gpa < 0 || gpa > 4) { showFErr('GPA must be 0.00 – 4.00.'); return; }

  const btn = document.getElementById('form-btn');
  btn.disabled = true; btn.textContent = id ? 'Saving…' : 'Adding…';

  try {
    const res  = await fetch(id ? `${API}/api/students?id=${id}` : `${API}/api/students`, {
      method: id ? 'PUT' : 'POST',
      headers: {'Content-Type':'application/json'},
      body: JSON.stringify({ name, age, course, gpa })
    });
    const data = await res.json();
    if (data.ok) {
      closeModal();
      showToast(id ? 'Student updated!' : 'Student added!', 'success');
      await loadStudents();
    } else {
      showFErr(data.error || 'Something went wrong.');
    }
  } catch { showFErr('Network error. Try again.'); }

  btn.disabled = false;
  btn.textContent = id ? 'Save Changes' : 'Add Student';
}

function showFErr(msg) {
  const el = document.getElementById('form-error');
  el.textContent = msg; el.classList.remove('hidden');
}

/* ── Delete Confirm ──────────────────── */
function confirmDelete(id, name) {
  pendingDeleteId = id;
  document.getElementById('confirm-name').textContent = name;
  document.getElementById('confirm-overlay').classList.remove('hidden');
}
function closeConfirm()    { pendingDeleteId = null; document.getElementById('confirm-overlay').classList.add('hidden'); }
function closeConfirmOut(e){ if (e.target === document.getElementById('confirm-overlay')) closeConfirm(); }

document.getElementById('del-btn').addEventListener('click', async () => {
  if (!pendingDeleteId) return;
  const btn = document.getElementById('del-btn');
  btn.textContent = 'Deleting…'; btn.disabled = true;
  try {
    const res  = await fetch(`${API}/api/students?id=${pendingDeleteId}`, { method: 'DELETE' });
    const data = await res.json();
    if (data.ok) { closeConfirm(); showToast('Student deleted.', 'success'); await loadStudents(); }
    else showToast(data.error || 'Delete failed.', 'error');
  } catch { showToast('Network error.', 'error'); }
  btn.textContent = 'Delete'; btn.disabled = false;
});

/* ── Toast ───────────────────────────── */
let toastTimer;
function showToast(msg, type = 'success') {
  const t = document.getElementById('toast');
  t.textContent = msg; t.className = `toast ${type}`;
  clearTimeout(toastTimer);
  toastTimer = setTimeout(() => t.classList.add('hidden'), 3000);
}

/* ── Helpers ─────────────────────────── */
function esc(s) {
  return String(s)
    .replace(/&/g,'&amp;').replace(/</g,'&lt;')
    .replace(/>/g,'&gt;').replace(/"/g,'&quot;');
}

function gpaChip(gpa) {
  const cls = gpa >= 3.5 ? 'gpa-hi' : gpa >= 2.5 ? 'gpa-md' : 'gpa-lo';
  return `<span class="gpa-chip ${cls}">${Number(gpa).toFixed(2)}</span>`;
}

/* ── Keyboard ────────────────────────── */
document.addEventListener('keydown', e => {
  if (e.key === 'Escape') { closeModal(); closeConfirm(); }
});

/* ── Theme ───────────────────────────── */
function applyTheme(theme) {
  document.documentElement.setAttribute('data-theme', theme);
  const isDark = theme === 'dark';
  document.getElementById('icon-sun').classList.toggle('hidden', !isDark);
  document.getElementById('icon-moon').classList.toggle('hidden', isDark);
  localStorage.setItem('sms_theme', theme);
}

function toggleTheme() {
  const current = document.documentElement.getAttribute('data-theme') || 'dark';
  applyTheme(current === 'dark' ? 'light' : 'dark');
}

/* ── Init ────────────────────────────── */
applyTheme(localStorage.getItem('sms_theme') || 'dark');
if (isLoggedIn()) showApp();
