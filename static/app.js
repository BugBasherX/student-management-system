/* =============================================
   Student Management System — Frontend
   ============================================= */

const API = '';  /* same origin */
let students = [];
let sortCol  = 'id';
let sortDir  = 1;   /* 1 = asc, -1 = desc */
let pendingDeleteId = null;

/* =============================================
   Auth
   ============================================= */
function isLoggedIn() { return sessionStorage.getItem('sms_auth') === '1'; }

document.getElementById('login-form').addEventListener('submit', async e => {
  e.preventDefault();
  const btn = document.getElementById('login-btn');
  const err = document.getElementById('login-error');
  btn.textContent = 'Signing in…';
  btn.disabled = true;
  err.classList.add('hidden');

  try {
    const res = await fetch(`${API}/api/login`, {
      method: 'POST',
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({
        username: document.getElementById('username').value,
        password: document.getElementById('password').value
      })
    });
    const data = await res.json();
    if (data.ok) {
      sessionStorage.setItem('sms_auth', '1');
      showApp();
    } else {
      err.classList.remove('hidden');
    }
  } catch {
    err.textContent = 'Could not reach the server. Try again.';
    err.classList.remove('hidden');
  }
  btn.textContent = 'Sign In';
  btn.disabled = false;
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

/* =============================================
   Navigation
   ============================================= */
function showSection(name) {
  document.querySelectorAll('.section').forEach(s => s.classList.add('hidden'));
  document.querySelectorAll('.nav-item').forEach(a => a.classList.remove('active'));
  document.getElementById(`section-${name}`).classList.remove('hidden');
  document.querySelector(`.nav-item[onclick*="${name}"]`).classList.add('active');
}

/* =============================================
   Data
   ============================================= */
async function loadStudents() {
  try {
    const res  = await fetch(`${API}/api/students`);
    students   = await res.json();
    renderTable();
    renderDashboard();
  } catch {
    showToast('Failed to load students', 'error');
  }
}

/* =============================================
   Dashboard
   ============================================= */
function renderDashboard() {
  document.getElementById('stat-total').textContent = students.length;

  const avgGpa = students.length
    ? (students.reduce((s, x) => s + x.gpa, 0) / students.length).toFixed(2)
    : '0.00';
  document.getElementById('stat-avg-gpa').textContent = avgGpa;

  const courses = new Set(students.map(s => s.course)).size;
  document.getElementById('stat-courses').textContent = courses;

  const top = students.length ? Math.max(...students.map(s => s.gpa)).toFixed(2) : '0.00';
  document.getElementById('stat-top-gpa').textContent = top;

  /* Recent: last 5 */
  const recent = [...students].sort((a, b) => b.id - a.id).slice(0, 5);
  const tbody  = document.getElementById('recent-body');
  tbody.innerHTML = recent.map(s => `
    <tr>
      <td>${s.id}</td>
      <td>${esc(s.name)}</td>
      <td>${s.age}</td>
      <td>${esc(s.course)}</td>
      <td>${gpaBadge(s.gpa)}</td>
    </tr>`).join('');
}

/* =============================================
   Student table
   ============================================= */
function renderTable() {
  const q    = document.getElementById('search-input').value.toLowerCase().trim();
  let list   = students.filter(s =>
    !q ||
    s.name.toLowerCase().includes(q) ||
    s.course.toLowerCase().includes(q) ||
    String(s.id).includes(q)
  );

  /* Sort */
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
      <td><span style="color:var(--text-3);font-size:.8rem">#</span>${s.id}</td>
      <td><strong>${esc(s.name)}</strong></td>
      <td>${s.age}</td>
      <td>${esc(s.course)}</td>
      <td>${gpaBadge(s.gpa)}</td>
      <td>
        <div class="action-btns">
          <button class="btn-icon btn-edit" onclick="openModal(${s.id})" title="Edit">
            <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
              <path d="M11 4H4a2 2 0 0 0-2 2v14a2 2 0 0 0 2 2h14a2 2 0 0 0 2-2v-7"/>
              <path d="M18.5 2.5a2.121 2.121 0 0 1 3 3L12 15l-4 1 1-4 9.5-9.5z"/>
            </svg>
          </button>
          <button class="btn-icon btn-delete" onclick="confirmDelete(${s.id}, '${esc(s.name)}')" title="Delete">
            <svg viewBox="0 0 24 24" fill="none" stroke="currentColor" stroke-width="2">
              <polyline points="3 6 5 6 21 6"/><path d="M19 6l-1 14a2 2 0 0 1-2 2H8a2 2 0 0 1-2-2L5 6"/>
              <path d="M10 11v6"/><path d="M14 11v6"/><path d="M9 6V4h6v2"/>
            </svg>
          </button>
        </div>
      </td>
    </tr>`).join('');
}

function sortBy(col) {
  if (sortCol === col) sortDir *= -1;
  else { sortCol = col; sortDir = 1; }
  /* Update icons */
  document.querySelectorAll('.sort-icon').forEach(el => {
    el.textContent = '↕';
    el.classList.remove('active');
  });
  const icon = document.querySelector(`.sort-icon[data-col="${col}"]`);
  if (icon) { icon.textContent = sortDir === 1 ? '↑' : '↓'; icon.classList.add('active'); }
  renderTable();
}

/* =============================================
   Add / Edit modal
   ============================================= */
function openModal(id = null) {
  const form  = document.getElementById('student-form');
  const title = document.getElementById('modal-title');
  const btn   = document.getElementById('form-submit-btn');
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
    title.textContent = 'Edit Student';
    btn.textContent   = 'Save Changes';
  } else {
    document.getElementById('edit-id').value = '';
    title.textContent = 'Add Student';
    btn.textContent   = 'Add Student';
  }
  document.getElementById('modal-overlay').classList.remove('hidden');
  setTimeout(() => document.getElementById('f-name').focus(), 50);
}

function closeModal() { document.getElementById('modal-overlay').classList.add('hidden'); }
function closeModalOutside(e) { if (e.target === document.getElementById('modal-overlay')) closeModal(); }

async function submitStudent(e) {
  e.preventDefault();
  const errEl = document.getElementById('form-error');
  errEl.classList.add('hidden');

  const id     = document.getElementById('edit-id').value;
  const name   = document.getElementById('f-name').value.trim();
  const age    = parseInt(document.getElementById('f-age').value);
  const course = document.getElementById('f-course').value.trim();
  const gpa    = parseFloat(document.getElementById('f-gpa').value);

  if (!name) { showFormError('Name is required.'); return; }
  if (isNaN(age) || age < 1) { showFormError('Enter a valid age.'); return; }
  if (isNaN(gpa) || gpa < 0 || gpa > 4) { showFormError('GPA must be between 0.00 and 4.00.'); return; }

  const btn = document.getElementById('form-submit-btn');
  btn.disabled = true;
  btn.textContent = id ? 'Saving…' : 'Adding…';

  try {
    const method = id ? 'PUT' : 'POST';
    const url    = id ? `${API}/api/students?id=${id}` : `${API}/api/students`;
    const res    = await fetch(url, {
      method,
      headers: { 'Content-Type': 'application/json' },
      body: JSON.stringify({ name, age, course, gpa })
    });
    const data = await res.json();
    if (data.ok) {
      closeModal();
      showToast(id ? 'Student updated!' : 'Student added!', 'success');
      await loadStudents();
    } else {
      showFormError(data.error || 'Something went wrong.');
    }
  } catch {
    showFormError('Network error. Try again.');
  }
  btn.disabled = false;
  btn.textContent = id ? 'Save Changes' : 'Add Student';
}

function showFormError(msg) {
  const el = document.getElementById('form-error');
  el.textContent = msg;
  el.classList.remove('hidden');
}

/* =============================================
   Delete confirmation
   ============================================= */
function confirmDelete(id, name) {
  pendingDeleteId = id;
  document.getElementById('confirm-name').textContent = name;
  document.getElementById('confirm-overlay').classList.remove('hidden');
}
function closeConfirm() {
  pendingDeleteId = null;
  document.getElementById('confirm-overlay').classList.add('hidden');
}
function closeConfirmOutside(e) { if (e.target === document.getElementById('confirm-overlay')) closeConfirm(); }

document.getElementById('confirm-delete-btn').addEventListener('click', async () => {
  if (!pendingDeleteId) return;
  const btn = document.getElementById('confirm-delete-btn');
  btn.textContent = 'Deleting…';
  btn.disabled = true;
  try {
    const res  = await fetch(`${API}/api/students?id=${pendingDeleteId}`, { method: 'DELETE' });
    const data = await res.json();
    if (data.ok) {
      closeConfirm();
      showToast('Student deleted.', 'success');
      await loadStudents();
    } else {
      showToast(data.error || 'Delete failed.', 'error');
    }
  } catch {
    showToast('Network error.', 'error');
  }
  btn.textContent = 'Delete';
  btn.disabled = false;
});

/* =============================================
   Toast
   ============================================= */
let toastTimer;
function showToast(msg, type = 'success') {
  const t = document.getElementById('toast');
  t.textContent = msg;
  t.className = `toast ${type}`;
  clearTimeout(toastTimer);
  toastTimer = setTimeout(() => t.classList.add('hidden'), 3000);
}

/* =============================================
   Helpers
   ============================================= */
function esc(str) {
  return String(str)
    .replace(/&/g, '&amp;')
    .replace(/</g, '&lt;')
    .replace(/>/g, '&gt;')
    .replace(/"/g, '&quot;');
}

function gpaBadge(gpa) {
  const cls = gpa >= 3.5 ? 'gpa-high' : gpa >= 2.5 ? 'gpa-mid' : 'gpa-low';
  return `<span class="gpa-badge ${cls}">${Number(gpa).toFixed(2)}</span>`;
}

/* =============================================
   Keyboard shortcuts
   ============================================= */
document.addEventListener('keydown', e => {
  if (e.key === 'Escape') {
    closeModal();
    closeConfirm();
  }
});

/* =============================================
   Init
   ============================================= */
if (isLoggedIn()) showApp();
