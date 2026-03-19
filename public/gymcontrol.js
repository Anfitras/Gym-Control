/* ============================================================
   GYMCONTROL — gymcontrol.js
   ============================================================ */

const API_BASE = 'http://localhost:8080/api';

/* ============================================================ API */
async function api(method, endpoint, body = null) {
  const opts = { method, headers: { 'Content-Type': 'application/json' } };
  if (body) opts.body = JSON.stringify(body);
  try {
    const res = await fetch(`${API_BASE}${endpoint}`, opts);
    return await res.json();
  } catch (err) {
    showToast('Erro ao conectar com o servidor C.', 'danger');
    return null;
  }
}
const GET  = (ep)       => api('GET',    ep);
const POST = (ep, body) => api('POST',   ep, body);
const PUT  = (ep, body) => api('PUT',    ep, body);
const DEL  = (ep)       => api('DELETE', ep);

/* ============================================================ SIDEBAR */
function buildSidebar(activeItem) {
  const navItems = [
    { id: 'dashboard',   href: 'dashboard.html',   icon: '📊', label: 'Dashboard' },
    { id: 'alunos',      href: 'alunos.html',       icon: '👥', label: 'Alunos' },
    { id: 'professores', href: 'professores.html',  icon: '🎓', label: 'Professores' },
    { id: 'turmas',      href: 'turmas.html',       icon: '📋', label: 'Turmas' },
    { id: 'controle',    href: 'controle.html',     icon: '🔗', label: 'Controle de Turmas' },
    { id: 'fichas',      href: 'fichas.html',       icon: '📝', label: 'Fichas de Treino' },
    { id: 'agenda',      href: 'agenda.html',       icon: '📅', label: 'Agenda' },
  ];
  const sidebar = document.getElementById('sidebar');
  if (!sidebar) return;
  sidebar.innerHTML = `
    <div class="sidebar-logo">
      <div class="logo-text"><span class="logo-icon">🏋️</span> GymControl</div>
      <div class="logo-sub">Gerenciamento de Academia</div>
    </div>
    <nav class="sidebar-nav">
      <div class="nav-section-label">Menu Principal</div>
      <ul>
        ${navItems.map(item => `
          <li class="nav-item ${activeItem === item.id ? 'active' : ''}">
            <a href="${item.href}">
              <span class="nav-icon">${item.icon}</span>
              <span>${item.label}</span>
            </a>
          </li>`).join('')}
      </ul>
      <div class="nav-section-label" style="margin-top:16px">Sistema</div>
      <ul>
        <li class="nav-item">
          <a href="login.html"><span class="nav-icon">🚪</span><span>Sair</span></a>
        </li>
      </ul>
    </nav>
    <div class="sidebar-footer">
      <div class="sidebar-user">
        <div class="avatar">A</div>
        <div class="user-info">
          <div class="name">Administrador</div>
          <div class="role">Recepção / Admin</div>
        </div>
      </div>
    </div>`;
}

/* ============================================================ TOPBAR */
function buildTopbar(title) {
  const now = new Date();
  const dateStr = now.toLocaleDateString('pt-BR',
    { weekday: 'long', year: 'numeric', month: 'long', day: 'numeric' });
  const el = document.getElementById('topbar-date');
  if (el) el.textContent = dateStr;
  const t = document.getElementById('page-title');
  if (t) t.textContent = title;
}

/* ============================================================ MODAL */
function openModal(id)  { const m = document.getElementById(id); if (m) m.classList.add('open'); }
function closeModal(id) { const m = document.getElementById(id); if (m) m.classList.remove('open'); }

/* ============================================================ TOAST */
function showToast(msg, type = 'success') {
  const t = document.createElement('div');
  t.className = `alert alert-${type}`;
  t.style.cssText = 'position:fixed;bottom:24px;right:24px;z-index:9999;min-width:300px;box-shadow:0 4px 16px rgba(0,0,0,.2)';
  t.innerHTML = `<span>${type === 'success' ? '✅' : '❌'}</span> ${msg}`;
  document.body.appendChild(t);
  setTimeout(() => t.remove(), 3500);
}

/* ============================================================ HELPERS */
function _badge(status) {
  const cls = { 'Ativo':'badge-success',1:'badge-success','Inativo':'badge-danger',0:'badge-danger','Pendente':'badge-warning',2:'badge-warning' };
  const label = typeof status === 'number' ? (['Inativo','Ativo','Pendente'][status] ?? status) : status;
  return `<span class="badge ${cls[status] ?? 'badge-neutral'}">${label}</span>`;
}
function _nomeProfessor(id) {
  const p = (window._cacheProfessores || []).find(x => x.id === id);
  return p ? p.nome : (id ? `Prof. #${id}` : '—');
}
function _nomeTurma(id) {
  const t = (window._cacheTurmas || []).find(x => x.id === id);
  return t ? t.nome : (id ? `Turma #${id}` : '—');
}
function _nomeAluno(id) {
  const a = (window._cacheAlunos || []).find(x => x.id === id);
  return a ? a.nome : (id ? `Aluno #${id}` : '—');
}
function _vazio(tbodyId, colspan, msg) {
  const el = typeof tbodyId === 'string' ? document.getElementById(tbodyId) : tbodyId;
  if (el) el.innerHTML = `<tr><td colspan="${colspan}" style="text-align:center;color:var(--gray-500);padding:32px">${msg}</td></tr>`;
}
function _popularSelect(el, itens, valorKey, labelFn, placeholder) {
  if (!el) return;
  el.innerHTML = `<option value="0">${placeholder}</option>` +
    (itens || []).map(i => `<option value="${i[valorKey]}">${labelFn(i)}</option>`).join('');
}
async function _carregarCaches() {
  const [profs, alunos, turmas] = await Promise.all([
    GET('/professores'), GET('/alunos'), GET('/turmas')
  ]);
  window._cacheProfessores = profs  || [];
  window._cacheAlunos      = alunos || [];
  window._cacheTurmas      = turmas || [];
}

/* ============================================================ DASHBOARD */
async function carregarDashboard() {
  await _carregarCaches();
  const dash = await GET('/dashboard');
  if (dash) {
    const set = (attr, val) => { const el = document.querySelector(`[data-stat="${attr}"]`); if (el) el.textContent = val; };
    set('total_alunos',      dash.total_alunos);
    set('total_professores', dash.total_professores);
    set('total_turmas',      dash.total_turmas);
    set('total_fichas',      dash.total_fichas  ?? 0);
    set('total_agenda',      dash.total_agenda  ?? 0);
  }
  const tbAlunos = document.getElementById('tbody-alunos-recentes');
  if (tbAlunos) {
    const alunos = window._cacheAlunos;
    if (!alunos.length) { _vazio(tbAlunos, 4, 'Nenhum aluno cadastrado'); }
    else {
      tbAlunos.innerHTML = alunos.slice(-5).reverse().map(a => `
        <tr><td>${a.nome}</td><td>${_nomeTurma(a.id_turma)}</td><td>${a.data_matricula}</td><td>${_badge(a.status)}</td></tr>`).join('');
    }
  }
  const agenda = await GET('/agenda');
  const tbAgenda = document.getElementById('tbody-agenda-hoje');
  if (tbAgenda) {
    if (!agenda || !agenda.length) { _vazio(tbAgenda, 4, 'Nenhuma atividade agendada'); }
    else {
      tbAgenda.innerHTML = agenda.slice(0,5).map(ag => `
        <tr><td>${_nomeProfessor(ag.id_professor)}</td><td>${ag.atividade}</td><td>${ag.hora_inicio}</td><td><span class="badge badge-blue">${_nomeTurma(ag.id_turma)}</span></td></tr>`).join('');
    }
  }
  const grid = document.getElementById('grid-turmas');
  if (grid) {
    const turmas = window._cacheTurmas;
    if (!turmas.length) { grid.innerHTML = '<div style="color:var(--gray-500);font-size:13px;padding:8px">Nenhuma turma cadastrada</div>'; }
    else {
      grid.innerHTML = turmas.map(t => `
        <div style="background:var(--gray-50);border-radius:10px;padding:16px;border:1px solid var(--gray-200)">
          <div style="font-weight:600;font-size:13.5px;margin-bottom:4px">${t.nome}</div>
          <div style="font-size:12px;color:var(--gray-500);margin-bottom:10px">${_nomeProfessor(t.id_professor)} · ${t.horario}</div>
          <div style="background:var(--gray-200);border-radius:99px;height:6px;margin-bottom:6px">
            <div style="background:${t.ocupacao_pct>=80?'var(--warning)':'var(--blue-500)'};border-radius:99px;height:6px;width:${t.ocupacao_pct}%"></div>
          </div>
          <div style="font-size:12px;color:var(--gray-500)">${t.total_alunos} / ${t.capacidade_max} alunos</div>
        </div>`).join('');
    }
  }
}

/* ============================================================
   ALUNOS — com filtros e ícone de olho
   ============================================================ */
window._todosAlunos = [];

async function carregarAlunos() {
  await _carregarCaches();
  window._todosAlunos = window._cacheAlunos;

  // Popula filtro de turma
  const filtroTurma = document.getElementById('filtro-turma');
  if (filtroTurma) {
    filtroTurma.innerHTML = '<option value="">Todas as turmas</option>' +
      (window._cacheTurmas || []).map(t => `<option value="${t.id}">${t.nome}</option>`).join('');
  }

  renderizarAlunos(window._todosAlunos);
}

function filtrarAlunos() {
  const busca   = (document.getElementById('busca-alunos')?.value || '').toLowerCase();
  const status  = document.getElementById('filtro-status')?.value  || '';
  const turmaId = document.getElementById('filtro-turma')?.value   || '';

  const filtrados = window._todosAlunos.filter(a => {
    const textoAluno = `${a.nome} ${_nomeTurma(a.id_turma)} ${_nomeProfessor(a.id_professor)}`.toLowerCase();
    const okBusca  = !busca   || textoAluno.includes(busca);
    const statusAluno = String(a.status ?? '').toLowerCase();
    const statusFiltro = status.toLowerCase();
    const okStatus = !status ||
      statusAluno === statusFiltro ||
      (statusFiltro === 'inativo' && String(a.status) === '0') ||
      (statusFiltro === 'ativo' && String(a.status) === '1') ||
      (statusFiltro === 'pendente' && String(a.status) === '2');
    const okTurma  = !turmaId || String(a.id_turma) === turmaId;
    return okBusca && okStatus && okTurma;
  });

  renderizarAlunos(filtrados);
}

function renderizarAlunos(lista) {
  const total = document.getElementById('total-alunos');
  const info  = document.getElementById('info-registros');
  if (total) total.textContent = `${window._todosAlunos.length} aluno(s)`;
  if (info)  info.textContent  = `Exibindo ${lista.length} de ${window._todosAlunos.length} registros`;

  const tbody = document.getElementById('lista-alunos');
  if (!tbody) return;
  if (!lista.length) {
    _vazio('lista-alunos', 8, 'Nenhum aluno encontrado para os filtros selecionados.');
    return;
  }
  tbody.innerHTML = lista.map(a => `
    <tr>
      <td><strong>#${String(a.id).padStart(3,'0')}</strong></td>
      <td>${a.nome}</td>
      <td>${_nomeTurma(a.id_turma)}</td>
      <td>${_nomeProfessor(a.id_professor)}</td>
      <td>${a.telefone || '—'}</td>
      <td>${a.data_matricula}</td>
      <td>${_badge(a.status)}</td>
      <td class="td-actions">
        <button class="btn btn-outline btn-sm btn-icon" onclick="verAluno(${a.id})" title="Ver dados">👁️</button>
        <button class="btn btn-outline btn-sm btn-icon" onclick="editarAluno(${a.id})" title="Editar">✏️</button>
        <button class="btn btn-danger btn-sm btn-icon"  onclick="excluirAluno(${a.id})" title="Excluir">🗑️</button>
      </td>
    </tr>`).join('');
}

/* Ver dados completos do aluno */
async function verAluno(id) {
  const a = await GET(`/alunos?id=${id}`);
  if (!a) { showToast('Aluno não encontrado.', 'danger'); return; }
  const sexoFonte = a.sexo ?? (window._todosAlunos || []).find(x => x.id === id)?.sexo ?? '';
  const sexoRaw = String(sexoFonte).trim().toLowerCase();
  const sexoNum = Number(sexoFonte);
  const sexoStr = (sexoNum === 1 || sexoRaw === '1' || sexoRaw === 'masculino') ? 'Masculino'
                : (sexoNum === 2 || sexoRaw === '2' || sexoRaw === 'feminino')  ? 'Feminino'
                : 'Outro';
  const imc = (a.peso_kg>0&&a.altura_m>0)
    ? (a.peso_kg/(a.altura_m*a.altura_m)).toFixed(1) : '—';
  const campo = (label, valor) =>
    `<div>
      <div style="font-size:11px;font-weight:600;text-transform:uppercase;color:var(--gray-500);margin-bottom:3px">${label}</div>
      <div style="font-size:14px">${valor||'—'}</div>
    </div>`;
  const body = document.getElementById('ver-aluno-body');
  if (body) {
    body.innerHTML = `
      <div style="background:var(--blue-900);padding:20px 24px;color:white;display:flex;align-items:center;gap:16px">
        <div style="width:52px;height:52px;border-radius:50%;background:var(--blue-500);
             display:flex;align-items:center;justify-content:center;font-size:20px;font-weight:700;flex-shrink:0">
          ${a.nome ? a.nome.charAt(0).toUpperCase() : '?'}
        </div>
        <div style="flex:1">
          <div style="font-family:'Sora',sans-serif;font-size:17px;font-weight:700">${a.nome}</div>
          <div style="font-size:12px;opacity:.65;margin-top:3px">#${String(a.id).padStart(3,'0')} · ${_nomeTurma(a.id_turma)}</div>
        </div>
        <div>${_badge(a.status)}</div>
      </div>
      <div style="padding:20px 24px;display:grid;grid-template-columns:1fr 1fr;gap:16px">
        ${campo('CPF', a.cpf)}
        ${campo('Idade', a.idade ? a.idade+' anos' : '—')}
        ${campo('Sexo', sexoStr)}
        ${campo('Telefone', a.telefone)}
        ${campo('E-mail', a.email)}
        ${campo('Matrícula', a.data_matricula)}
        ${campo('Professor', _nomeProfessor(a.id_professor))}
        ${campo('Turma', _nomeTurma(a.id_turma))}
      </div>
      <div style="padding:0 24px 24px;display:grid;grid-template-columns:1fr 1fr 1fr;gap:12px">
        <div style="background:var(--blue-50);border-radius:10px;padding:14px;text-align:center">
          <div style="font-size:22px;font-weight:700;color:var(--blue-500)">${a.peso_kg||'—'}</div>
          <div style="font-size:12px;color:var(--gray-500)">kg</div>
        </div>
        <div style="background:var(--blue-50);border-radius:10px;padding:14px;text-align:center">
          <div style="font-size:22px;font-weight:700;color:var(--blue-500)">${a.altura_m||'—'}</div>
          <div style="font-size:12px;color:var(--gray-500)">m</div>
        </div>
        <div style="background:var(--blue-50);border-radius:10px;padding:14px;text-align:center">
          <div style="font-size:22px;font-weight:700;color:var(--blue-500)">${imc}</div>
          <div style="font-size:12px;color:var(--gray-500)">IMC</div>
        </div>
      </div>`;
  }
  const btnEditar = document.getElementById('btn-editar-aluno-ver');
  if (btnEditar) btnEditar.onclick = () => { closeModal('modal-ver-aluno'); editarAluno(id); };
  openModal('modal-ver-aluno');
}

function abrirModalNovoAluno() {
  document.getElementById('aluno-id').value = '';
  document.getElementById('aluno-codigo').value = 'Gerado automaticamente';
  ['aluno-nome','aluno-cpf','aluno-telefone','aluno-email','aluno-data-matricula'].forEach(id => {
    const el = document.getElementById(id); if (el) el.value = '';
  });
  ['aluno-idade','aluno-peso','aluno-altura'].forEach(id => {
    const el = document.getElementById(id); if (el) el.value = '';
  });
  const s = document.getElementById('aluno-status'); if (s) s.value = '1';
  const sx = document.getElementById('aluno-sexo');  if (sx) sx.value = '1';
  _popularSelect(document.getElementById('aluno-turma'),    window._cacheTurmas,      'id', t => t.nome, 'Selecionar turma...');
  _popularSelect(document.getElementById('aluno-professor'), window._cacheProfessores, 'id', p => p.nome, 'Selecionar professor...');
  const titulo = document.getElementById('modal-aluno-titulo');
  if (titulo) titulo.textContent = '➕ Cadastrar Novo Aluno';
  openModal('modal-aluno');
}

async function editarAluno(id) {
  await _carregarCaches();
  const a = await GET(`/alunos?id=${id}`);
  if (!a) return;
  document.getElementById('aluno-id').value             = a.id;
  document.getElementById('aluno-codigo').value         = `#${String(a.id).padStart(3,'0')}`;
  document.getElementById('aluno-nome').value           = a.nome           || '';
  document.getElementById('aluno-cpf').value            = a.cpf            || '';
  document.getElementById('aluno-telefone').value       = a.telefone       || '';
  document.getElementById('aluno-email').value          = a.email          || '';
  document.getElementById('aluno-data-matricula').value = a.data_matricula || '';
  document.getElementById('aluno-idade').value          = a.idade          || '';
  document.getElementById('aluno-peso').value           = a.peso_kg        || '';
  document.getElementById('aluno-altura').value         = a.altura_m       || '';
  document.getElementById('aluno-sexo').value           = a.sexo           || 1;
  document.getElementById('aluno-status').value         = a.status === 'Ativo' ? 1 : a.status === 'Pendente' ? 2 : 0;
  _popularSelect(document.getElementById('aluno-turma'),    window._cacheTurmas,      'id', t => t.nome, 'Sem turma');
  _popularSelect(document.getElementById('aluno-professor'), window._cacheProfessores, 'id', p => p.nome, 'Sem professor');
  const selT = document.getElementById('aluno-turma');    if (selT) selT.value = a.id_turma    || 0;
  const selP = document.getElementById('aluno-professor'); if (selP) selP.value = a.id_professor || 0;
  const titulo = document.getElementById('modal-aluno-titulo');
  if (titulo) titulo.textContent = '✏️ Editar Aluno';
  openModal('modal-aluno');
}

async function salvarAluno() {
  const nome = document.getElementById('aluno-nome')?.value.trim();
  if (!nome) { showToast('Nome é obrigatório!', 'danger'); return; }
  const statusRaw = document.getElementById('aluno-status')?.value;
  const statusNum = statusRaw === undefined || statusRaw === null || statusRaw === ''
    ? 1
    : Number(statusRaw);

  const body = {
    nome,
    cpf:            document.getElementById('aluno-cpf')?.value.trim()           || '',
    telefone:       document.getElementById('aluno-telefone')?.value.trim()       || '',
    email:          document.getElementById('aluno-email')?.value.trim()          || '',
    data_matricula: document.getElementById('aluno-data-matricula')?.value.trim() || '',
    idade:          Number(document.getElementById('aluno-idade')?.value)         || 0,
    sexo:           Number(document.getElementById('aluno-sexo')?.value)          || 1,
    peso_kg:        Number(document.getElementById('aluno-peso')?.value)          || 0,
    altura_m:       Number(document.getElementById('aluno-altura')?.value)        || 0,
    id_turma:       Number(document.getElementById('aluno-turma')?.value)         || 0,
    id_professor:   Number(document.getElementById('aluno-professor')?.value)     || 0,
    status:         Number.isNaN(statusNum) ? 1 : statusNum,
  };
  const id = document.getElementById('aluno-id')?.value;
  const res = id ? await PUT(`/alunos?id=${id}`, body) : await POST('/alunos', body);
  if (res?.ok) {
    closeModal('modal-aluno');
    showToast(id ? 'Aluno atualizado!' : 'Aluno cadastrado!');
    await _carregarCaches();
    carregarAlunos();
  } else { showToast('Erro ao salvar aluno.', 'danger'); }
}

async function excluirAluno(id) {
  if (!confirm('Deseja excluir este aluno?')) return;
  const res = await DEL(`/alunos?id=${id}`);
  if (res?.ok) { showToast('Aluno removido.'); await _carregarCaches(); carregarAlunos(); }
  else showToast('Erro ao remover.', 'danger');
}

/* ============================================================ PROFESSORES */
window._todosProfessores = [];

async function carregarProfessores() {
  await _carregarCaches();
  window._todosProfessores = window._cacheProfessores || [];
  renderizarProfessores(window._todosProfessores);
}

function filtrarProfessores() {
  const busca = (document.getElementById('busca-prof')?.value || '').toLowerCase();
  const espSelecionada = document.getElementById('filtro-especialidade-prof')?.value || '';

  const norm = (s) => String(s || '')
    .normalize('NFD')
    .replace(/[\u0300-\u036f]/g, '')
    .toLowerCase()
    .trim();

  const espFiltro = norm(espSelecionada);

  const filtrados = (window._todosProfessores || []).filter(p => {
    const texto = `${p.nome} ${p.especialidade} ${p.email || ''} ${p.telefone || ''}`.toLowerCase();
    const okBusca = !busca || texto.includes(busca);
    const okEspecialidade = !espFiltro || norm(p.especialidade) === espFiltro;
    return okBusca && okEspecialidade;
  });

  renderizarProfessores(filtrados);
}

function renderizarProfessores(profs) {
  const tbody = document.getElementById('lista-professores');
  if (tbody) {
    if (!profs.length) { _vazio('lista-professores', 6, 'Nenhum professor cadastrado.'); }
    else {
      tbody.innerHTML = profs.map(p => `
        <tr>
          <td><strong>P${String(p.id).padStart(3,'0')}</strong></td>
          <td>${p.nome}</td>
          <td>${p.especialidade}</td>
          <td>${p.horario_disponivel || '—'}</td>
          <td>${_badge(p.status)}</td>
          <td class="td-actions">
            <button class="btn btn-outline btn-sm btn-icon" onclick="editarProfessor(${p.id})">✏️</button>
            <button class="btn btn-danger btn-sm btn-icon"  onclick="excluirProfessor(${p.id})">🗑️</button>
          </td>
        </tr>`).join('');
    }
  }
  const grid = document.getElementById('grid-professores');
  if (grid) {
    if (!profs.length) { grid.innerHTML = '<div style="color:var(--gray-500);font-size:13px;padding:8px">Nenhum professor cadastrado.</div>'; }
    else {
      const cores = ['var(--blue-600)','#7c3aed','#059669','#d97706','#475569'];
      grid.innerHTML = profs.map((p, i) => `
        <div class="card" style="padding:24px">
          <div style="display:flex;gap:14px;align-items:flex-start;margin-bottom:14px">
            <div style="width:50px;height:50px;border-radius:50%;background:${cores[i%5]};display:flex;align-items:center;justify-content:center;font-size:20px;color:white;font-weight:700;flex-shrink:0">${p.nome.charAt(0).toUpperCase()}</div>
            <div>
              <div style="font-family:'Sora',sans-serif;font-weight:600;font-size:15px">${p.nome}</div>
              <div style="font-size:12.5px;color:var(--gray-500)">Especialidade: <strong>${p.especialidade}</strong></div>
              ${_badge(p.status)}
            </div>
          </div>
          <div style="font-size:13px;color:var(--gray-700);display:flex;flex-direction:column;gap:6px;border-top:1px solid var(--gray-100);padding-top:14px">
            <div>📞 ${p.telefone || '—'}</div>
            <div>📧 ${p.email    || '—'}</div>
            <div>🕐 ${p.horario_disponivel || '—'}</div>
          </div>
          <div style="display:flex;gap:8px;margin-top:14px">
            <button class="btn btn-outline btn-sm" style="flex:1" onclick="editarProfessor(${p.id})">✏️ Editar</button>
            <button class="btn btn-danger btn-sm btn-icon" onclick="excluirProfessor(${p.id})">🗑️</button>
          </div>
        </div>`).join('');
    }
  }
}

function abrirModalNovoProfessor() {
  document.getElementById('prof-id').value = '';
  ['prof-nome','prof-cpf','prof-telefone','prof-email','prof-horario'].forEach(id => { const el=document.getElementById(id); if(el) el.value=''; });
  const s = document.getElementById('prof-status'); if(s) s.value='1';
  const titulo = document.getElementById('modal-prof-titulo');
  if (titulo) titulo.textContent = '➕ Cadastrar Professor';
  openModal('modal-prof');
}

async function editarProfessor(id) {
  const p = (window._cacheProfessores || []).find(x => x.id === id);
  if (!p) return;
  document.getElementById('prof-id').value       = p.id;
  document.getElementById('prof-nome').value     = p.nome               || '';
  document.getElementById('prof-cpf').value      = p.cpf                || '';
  document.getElementById('prof-telefone').value = p.telefone           || '';
  document.getElementById('prof-email').value    = p.email              || '';
  document.getElementById('prof-horario').value  = p.horario_disponivel || '';
  document.getElementById('prof-status').value   = p.status === 'Ativo' ? 1 : 0;
  const esp = document.getElementById('prof-especialidade');
  if (esp) esp.value = p.especialidade || 'Musculação';
  const titulo = document.getElementById('modal-prof-titulo');
  if (titulo) titulo.textContent = '✏️ Editar Professor';
  openModal('modal-prof');
}

async function salvarProfessor() {
  const nome = document.getElementById('prof-nome')?.value.trim();
  if (!nome) { showToast('Nome é obrigatório!', 'danger'); return; }
  const body = {
    nome,
    cpf:                document.getElementById('prof-cpf')?.value.trim()      || '',
    telefone:           document.getElementById('prof-telefone')?.value.trim() || '',
    email:              document.getElementById('prof-email')?.value.trim()    || '',
    especialidade:      document.getElementById('prof-especialidade')?.value   || '',
    horario_disponivel: document.getElementById('prof-horario')?.value.trim()  || '',
    status:             Number(document.getElementById('prof-status')?.value)  ?? 1,
  };
  const id = document.getElementById('prof-id')?.value;
  const res = id ? await PUT(`/professores?id=${id}`, body) : await POST('/professores', body);
  if (res?.ok) {
    closeModal('modal-prof');
    showToast(id ? 'Professor atualizado!' : 'Professor cadastrado!');
    await _carregarCaches(); carregarProfessores();
  } else showToast('Erro ao salvar.', 'danger');
}

async function excluirProfessor(id) {
  if (!confirm('Deseja excluir este professor?')) return;
  const res = await DEL(`/professores?id=${id}`);
  if (res?.ok) { showToast('Professor removido.'); await _carregarCaches(); carregarProfessores(); }
  else showToast('Erro ao remover.', 'danger');
}

/* ============================================================ TURMAS */
async function carregarTurmas() {
  await _carregarCaches();
  const turmas = window._cacheTurmas;
  const tbody = document.getElementById('lista-turmas');
  if (tbody) {
    if (!turmas.length) { _vazio('lista-turmas', 9, 'Nenhuma turma cadastrada.'); }
    else {
      tbody.innerHTML = turmas.map(t => `
        <tr>
          <td><strong>T${String(t.id).padStart(3,'0')}</strong></td>
          <td>${t.nome}</td>
          <td><span class="badge badge-neutral">${t.modalidade}</span></td>
          <td>${_nomeProfessor(t.id_professor)}</td>
          <td>${t.horario} ${t.dias_semana ? '· '+t.dias_semana : ''}</td>
          <td>${t.total_alunos} / ${t.capacidade_max}</td>
          <td>
            <div style="background:var(--gray-200);border-radius:99px;height:5px;width:80px">
              <div style="background:${t.ocupacao_pct>=80?'var(--warning)':'var(--blue-500)'};border-radius:99px;height:5px;width:${t.ocupacao_pct}%"></div>
            </div>
            <span style="font-size:11px;color:var(--gray-500)">${t.ocupacao_pct}%</span>
          </td>
          <td>${_badge(t.status)}</td>
          <td class="td-actions">
            <button class="btn btn-outline btn-sm btn-icon" onclick="editarTurma(${t.id})">✏️</button>
            <button class="btn btn-danger btn-sm btn-icon"  onclick="excluirTurma(${t.id})">🗑️</button>
          </td>
        </tr>`).join('');
    }
  }
  const grid = document.getElementById('grid-turmas-cards');
  if (grid) {
    if (!turmas.length) { grid.innerHTML = '<div style="color:var(--gray-500);font-size:13px;padding:8px">Nenhuma turma cadastrada.</div>'; }
    else {
      const cores = { 'Musculação':'var(--blue-900)','Funcional':'#059669','Spinning':'#7c3aed','Zumba':'#d97706' };
      grid.innerHTML = turmas.map(t => {
        const cor = cores[t.modalidade] || 'var(--blue-900)';
        const statusClass = t.status === 'Inativo' ? 'badge-danger' : t.status === 'Pendente' ? 'badge-warning' : 'badge-success';
        const statusLabel = t.status === 'Inativo' ? 'Inativa' : t.status === 'Pendente' ? 'Pendente' : 'Ativa';
        const badgeOcup = `<span class="badge ${statusClass}">${t.ocupacao_pct >= 80 ? 'Quase cheia' : statusLabel}</span>`;
        return `
          <div class="card">
            <div style="background:${cor};padding:18px 20px;border-radius:var(--radius-lg) var(--radius-lg) 0 0">
              <div style="display:flex;align-items:center;justify-content:space-between;margin-bottom:8px">
                <span class="badge" style="background:rgba(255,255,255,.15);color:white">${t.modalidade}</span>${badgeOcup}
              </div>
              <div style="font-family:'Sora',sans-serif;font-size:17px;font-weight:700;color:white">${t.nome}</div>
              <div style="font-size:12.5px;color:rgba(255,255,255,.65);margin-top:3px">${_nomeProfessor(t.id_professor)} · ${t.horario}</div>
            </div>
            <div style="padding:16px 20px">
              <div style="display:flex;justify-content:space-between;margin-bottom:8px">
                <span style="font-size:13px;color:var(--gray-500)">Alunos</span>
                <span style="font-size:13px;font-weight:600">${t.total_alunos} / ${t.capacidade_max}</span>
              </div>
              <div style="background:var(--gray-200);border-radius:99px;height:7px;margin-bottom:14px">
                <div style="background:${t.ocupacao_pct>=80?'var(--warning)':'var(--blue-500)'};border-radius:99px;height:7px;width:${t.ocupacao_pct}%"></div>
              </div>
              <div style="display:flex;gap:8px">
                <button class="btn btn-outline btn-sm" style="flex:1" onclick="editarTurma(${t.id})">✏️ Editar</button>
                <button class="btn btn-primary btn-sm" style="flex:1" onclick="verAlunosTurma(${t.id},'${t.nome}')">👥 Alunos</button>
              </div>
            </div>
          </div>`;
      }).join('');
    }
  }
}

function abrirModalNovaTurma() {
  document.getElementById('turma-id').value = '';
  ['turma-nome','turma-horario','turma-dias'].forEach(id => { const el=document.getElementById(id); if(el) el.value=''; });
  const c = document.getElementById('turma-capacidade'); if(c) c.value='';
  const s = document.getElementById('turma-status'); if(s) s.value='1';
  _popularSelect(document.getElementById('turma-professor'), window._cacheProfessores, 'id', p => p.nome, 'Selecionar professor...');
  const titulo = document.getElementById('modal-turma-titulo');
  if (titulo) titulo.textContent = '➕ Cadastrar Turma';
  openModal('modal-turma');
}

async function editarTurma(id) {
  await _carregarCaches();
  const t = (window._cacheTurmas || []).find(x => x.id === id);
  if (!t) return;
  document.getElementById('turma-id').value         = t.id;
  document.getElementById('turma-nome').value       = t.nome           || '';
  document.getElementById('turma-horario').value    = t.horario        || '';
  document.getElementById('turma-dias').value       = t.dias_semana    || '';
  document.getElementById('turma-capacidade').value = t.capacidade_max || '';
  document.getElementById('turma-status').value     = t.status === 'Ativo' ? '1' : t.status === 'Pendente' ? '2' : '0';
  const mod = document.getElementById('turma-modalidade'); if(mod) mod.value = t.modalidade || 'Musculação';
  _popularSelect(document.getElementById('turma-professor'), window._cacheProfessores, 'id', p => p.nome, 'Selecionar professor...');
  const selP = document.getElementById('turma-professor'); if(selP) selP.value = t.id_professor || 0;
  const titulo = document.getElementById('modal-turma-titulo');
  if (titulo) titulo.textContent = '✏️ Editar Turma';
  openModal('modal-turma');
}

async function salvarTurma() {
  const nome = document.getElementById('turma-nome')?.value.trim();
  if (!nome) { showToast('Nome é obrigatório!', 'danger'); return; }
  const body = {
    nome,
    modalidade:     document.getElementById('turma-modalidade')?.value         || '',
    id_professor:   Number(document.getElementById('turma-professor')?.value)  || 0,
    horario:        document.getElementById('turma-horario')?.value.trim()     || '',
    dias_semana:    document.getElementById('turma-dias')?.value.trim()        || '',
    capacidade_max: Number(document.getElementById('turma-capacidade')?.value) || 30,
    status:         Number(document.getElementById('turma-status')?.value) ?? 1,
  };
  const id = document.getElementById('turma-id')?.value;
  const res = id ? await PUT(`/turmas?id=${id}`, body) : await POST('/turmas', body);
  if (res?.ok) {
    closeModal('modal-turma');
    showToast(id ? 'Turma atualizada!' : 'Turma cadastrada!');
    await _carregarCaches(); carregarTurmas();
  } else showToast('Erro ao salvar.', 'danger');
}

async function excluirTurma(id) {
  if (!confirm('Deseja excluir esta turma?')) return;
  const res = await DEL(`/turmas?id=${id}`);
  if (res?.ok) { showToast('Turma removida.'); await _carregarCaches(); carregarTurmas(); }
  else showToast('Erro ao remover.', 'danger');
}

async function verAlunosTurma(idTurma, nomeTurma) {
  const tbody  = document.getElementById('lista-alunos-turma');
  const titulo = document.getElementById('modal-ver-turma-titulo');
  if (titulo) titulo.textContent = `👥 Alunos — ${nomeTurma}`;
  const alunos = (window._cacheAlunos || []).filter(a => a.id_turma === idTurma);
  if (tbody) {
    if (!alunos.length) { _vazio(tbody, 5, 'Nenhum aluno nesta turma'); }
    else {
      tbody.innerHTML = alunos.map((a,i) => `
        <tr><td>${i+1}</td><td>${a.nome}</td><td>${a.telefone||'—'}</td><td>${a.data_matricula}</td><td>${_badge(a.status)}</td></tr>`).join('');
    }
  }
  openModal('modal-ver-turma');
}

/* ============================================================
   FICHAS — filtros + até 6 exercícios dinâmicos + visualização
   ============================================================ */
window._todasFichas = [];

async function carregarFichas() {
  await _carregarCaches();
  const fichas = await GET('/fichas');
  window._todasFichas = (fichas || []).filter(f => Number(f.id_aluno) > 0 && Number(f.id_professor) > 0);
  renderizarFichas(window._todasFichas);
}

function filtrarFichas() {
  const busca    = (document.getElementById('busca-fichas')?.value || '').toLowerCase();
  const objetivo = document.getElementById('filtro-objetivo')?.value || '';

  const filtradas = window._todasFichas.filter(f => {
    const nomeAluno = _nomeAluno(f.id_aluno).toLowerCase();
    const okBusca   = !busca    || nomeAluno.includes(busca);
    const okObj     = !objetivo || f.objetivo === objetivo;
    return okBusca && okObj;
  });
  renderizarFichas(filtradas);
}

function renderizarFichas(lista) {
  const tbody = document.getElementById('lista-fichas');
  if (tbody) {
    if (!lista.length) { _vazio('lista-fichas', 7, 'Nenhuma ficha encontrada.'); }
    else {
      tbody.innerHTML = lista.map(f => `
        <tr>
          <td><strong>F${String(f.id).padStart(3,'0')}</strong></td>
          <td>${_nomeAluno(f.id_aluno)}</td>
          <td>${_nomeProfessor(f.id_professor)}</td>
          <td><span class="badge badge-blue">${f.objetivo}</span></td>
          <td>${f.total_exercicios} exercício(s)</td>
          <td>${f.data_criacao}</td>
          <td class="td-actions">
            <button class="btn btn-outline btn-sm btn-icon" onclick="verFicha(${f.id})" title="Ver exercícios">👁️</button>
            <button class="btn btn-danger btn-sm btn-icon"  onclick="excluirFicha(${f.id})" title="Excluir">🗑️</button>
          </td>
        </tr>`).join('');
    }
  }

  const grid = document.getElementById('grid-fichas-cards');
  if (grid) {
    if (!lista.length) { grid.innerHTML = '<div style="color:var(--gray-500);font-size:13px;padding:8px">Nenhuma ficha cadastrada.</div>'; }
    else {
      grid.innerHTML = lista.map(f => `
        <div class="ficha-card">
          <div class="ficha-header">
            <div style="display:flex;justify-content:space-between;align-items:flex-start">
              <div>
                <div class="ficha-title">${_nomeAluno(f.id_aluno)}</div>
                <div class="ficha-meta">${_nomeProfessor(f.id_professor)} · Ficha #${String(f.id).padStart(3,'0')}</div>
              </div>
              <span class="badge" style="background:rgba(255,255,255,.15);color:white">${f.objetivo}</span>
            </div>
          </div>
          <div class="ficha-exercicios">
            <div style="font-size:11px;font-weight:600;text-transform:uppercase;letter-spacing:.08em;color:var(--gray-500);margin-bottom:10px">
              ${f.total_exercicios} exercício(s)
            </div>
            ${(f.exercicios || []).slice(0,3).map(e => `
              <div class="exercicio-row">
                <div style="display:flex;align-items:center;gap:10px">
                  <div class="exercicio-num">${e.numero}</div>
                  <div><div class="exercicio-nome">${e.nome}</div><div style="font-size:12px;color:var(--gray-500)">${e.grupo_muscular}</div></div>
                </div>
                <div style="text-align:center"><div style="font-size:13px;font-weight:600">${e.series}</div><div style="font-size:11px;color:var(--gray-500)">séries</div></div>
                <div style="text-align:center"><div style="font-size:13px;font-weight:600">${e.repeticoes}</div><div style="font-size:11px;color:var(--gray-500)">reps</div></div>
              </div>`).join('')}
            ${f.total_exercicios > 3 ? `<div style="font-size:12px;color:var(--gray-500);text-align:center;padding:8px">+ ${f.total_exercicios-3} exercício(s)</div>` : ''}
            <div style="display:flex;gap:8px;margin-top:14px">
              <button class="btn btn-outline btn-sm" style="flex:1" onclick="verFicha(${f.id})">👁️ Ver todos exercícios</button>
              <button class="btn btn-danger btn-sm btn-icon" onclick="excluirFicha(${f.id})">🗑️</button>
            </div>
          </div>
        </div>`).join('');
    }
  }
}

/* Campos de exercício dinâmicos */
let _numExercicios = 0;

function _criarCampoExercicio(n) {
  return `
    <div id="exercicio-bloco-${n}" style="border:1px solid var(--gray-200);border-radius:var(--radius);padding:16px;margin-bottom:12px">
      <div style="display:flex;justify-content:space-between;align-items:center;margin-bottom:12px">
        <div style="font-weight:600;font-size:13px;color:var(--gray-700)">Exercício ${n}</div>
        ${n > 1 ? `<button class="btn btn-danger btn-sm btn-icon" onclick="removerExercicio(${n})" title="Remover">✕</button>` : ''}
      </div>
      <div class="form-group">
        <label class="form-label">Nome do Exercício</label>
        <input type="text" id="ex${n}-nome" class="form-control" placeholder="Ex: Supino Reto" />
      </div>
      <div class="form-row">
        <div class="form-group">
          <label class="form-label">Grupo Muscular</label>
          <input type="text" id="ex${n}-grupo" class="form-control" placeholder="Ex: Peito" />
        </div>
        <div class="form-group">
          <label class="form-label">Séries</label>
          <input type="number" id="ex${n}-series" class="form-control" placeholder="4" min="1" />
        </div>
        <div class="form-group">
          <label class="form-label">Repetições</label>
          <input type="number" id="ex${n}-reps" class="form-control" placeholder="12" min="1" />
        </div>
        <div class="form-group">
          <label class="form-label">Carga (kg)</label>
          <input type="number" id="ex${n}-carga" class="form-control" placeholder="0" step="0.5" />
        </div>
      </div>
    </div>`;
}

function adicionarCampoExercicio() {
  _numExercicios++;
  const container = document.getElementById('exercicios-container');
  if (container) container.insertAdjacentHTML('beforeend', _criarCampoExercicio(_numExercicios));
}

function removerExercicio(n) {
  const bloco = document.getElementById(`exercicio-bloco-${n}`);
  if (bloco) bloco.remove();
}

function abrirModalNovaFicha() {
  _popularSelect(document.getElementById('ficha-aluno'),     window._cacheAlunos,      'id', a => a.nome, 'Selecionar aluno...');
  _popularSelect(document.getElementById('ficha-professor'), window._cacheProfessores, 'id', p => p.nome, 'Selecionar professor...');
  const obs = document.getElementById('ficha-obs'); if(obs) obs.value='';
  const container = document.getElementById('exercicios-container');
  if (container) container.innerHTML = '';
  _numExercicios = 0;
  adicionarCampoExercicio(); // começa com 1
  openModal('modal-ficha');
}

async function salvarFicha() {
  const idAluno = Number(document.getElementById('ficha-aluno')?.value);
  const idProf  = Number(document.getElementById('ficha-professor')?.value);
  if (!idAluno || !idProf) { showToast('Selecione aluno e professor!', 'danger'); return; }

  const blocosEx = Array.from(document.querySelectorAll('[id^="exercicio-bloco-"]'));
  const exercicios = blocosEx
    .map(bloco => {
      const n = bloco.id.replace('exercicio-bloco-', '');
      const nome = document.getElementById(`ex${n}-nome`)?.value.trim() || '';
      if (!nome) return null;
      return {
        nome,
        grupo_muscular: document.getElementById(`ex${n}-grupo`)?.value.trim()  || '',
        series:         Number(document.getElementById(`ex${n}-series`)?.value) || 1,
        repeticoes:     Number(document.getElementById(`ex${n}-reps`)?.value)   || 1,
        carga_kg:       Number(document.getElementById(`ex${n}-carga`)?.value)  || 0,
        observacao:     '',
      };
    })
    .filter(Boolean);

  if (!exercicios.length) {
    showToast('Adicione pelo menos 1 exercício com nome.', 'danger');
    return;
  }

  const body = {
    id_aluno:     idAluno,
    id_professor: idProf,
    objetivo:     Number(document.getElementById('ficha-objetivo-modal')?.value) || 1,
    observacoes:  document.getElementById('ficha-obs')?.value.trim() || '',
  };
  const res = await POST('/fichas', body);
  if (!res?.ok) { showToast('Erro ao criar ficha.', 'danger'); return; }
  const idFicha = res.id;

  /* Envia todos os exercícios preenchidos e valida retorno */
  for (const ex of exercicios) {
    const exRes = await POST(`/fichas/${idFicha}/exercicio`, ex);
    if (!exRes?.ok) {
      showToast('Erro ao salvar exercícios da ficha.', 'danger');
      return;
    }
  }

  closeModal('modal-ficha');
  showToast('Ficha salva com sucesso!');
  await _carregarCaches();
  await carregarFichas();
}

/* Visualiza exercícios da ficha sem imprimir */
async function verFicha(id) {
  const data = await GET(`/fichas?id=${id}`);
  const f = Array.isArray(data) ? (data || []).find(x => x.id === id) : data;
  if (!f) return;

  const titulo = document.getElementById('modal-ver-ficha-titulo');
  if (titulo) titulo.textContent = `👁️ Ficha — ${_nomeAluno(f.id_aluno)}`;

  const body = document.getElementById('modal-ver-ficha-body');
  if (body) {
    body.innerHTML = `
      <div style="background:var(--blue-900);padding:20px 24px;color:white">
        <div style="display:flex;justify-content:space-between;align-items:flex-start">
          <div>
            <div style="font-family:'Sora',sans-serif;font-size:16px;font-weight:700">${_nomeAluno(f.id_aluno)}</div>
            <div style="font-size:12.5px;opacity:.6;margin-top:3px">Prof. ${_nomeProfessor(f.id_professor)}</div>
          </div>
          <div style="text-align:right">
            <div style="font-size:12px;opacity:.5">Ficha #${String(f.id).padStart(3,'0')}</div>
            <span class="badge" style="background:rgba(255,255,255,.15);color:white;margin-top:4px;display:inline-block">${f.objetivo}</span>
          </div>
        </div>
      </div>
      <div style="padding:20px 24px">
        ${f.total_exercicios === 0
          ? '<div style="color:var(--gray-500);text-align:center;padding:20px">Nenhum exercício cadastrado nesta ficha.</div>'
          : `
            <div style="font-size:11px;font-weight:700;text-transform:uppercase;letter-spacing:.08em;color:var(--gray-500);margin-bottom:12px">
              ${f.total_exercicios} Exercício(s)
            </div>
            ${(f.exercicios || []).map(e => `
              <div class="exercicio-row">
                <div style="display:flex;align-items:center;gap:10px">
                  <div class="exercicio-num">${e.numero}</div>
                  <div>
                    <div class="exercicio-nome">${e.nome}</div>
                    <div style="font-size:12px;color:var(--gray-500)">${e.grupo_muscular}</div>
                  </div>
                </div>
                <div style="text-align:center"><div style="font-weight:600">${e.series}</div><div style="font-size:11px;color:var(--gray-500)">séries</div></div>
                <div style="text-align:center"><div style="font-weight:600">${e.repeticoes}</div><div style="font-size:11px;color:var(--gray-500)">reps</div></div>
                <div style="text-align:center"><div style="font-weight:600">${e.carga_kg > 0 ? e.carga_kg+'kg' : 'Livre'}</div><div style="font-size:11px;color:var(--gray-500)">carga</div></div>
              </div>`).join('')}
          `}
        ${f.observacoes ? `
          <div style="margin-top:14px;padding:12px;background:var(--blue-50);border-radius:8px;font-size:13px;color:var(--blue-700)">
            💬 <strong>Observações:</strong> ${f.observacoes}
          </div>` : ''}
        <div style="margin-top:14px;font-size:12px;color:var(--gray-500)">Data de criação: ${f.data_criacao}</div>
      </div>`;
  }
  openModal('modal-ver-ficha');
}

async function excluirFicha(id) {
  if (!confirm('Deseja excluir esta ficha?')) return;
  const res = await DEL(`/fichas?id=${id}`);
  if (res?.ok) { showToast('Ficha removida.'); carregarFichas(); }
  else showToast('Erro ao remover.', 'danger');
}

/* ============================================================ CONTROLE */
async function carregarControle() {
  await _carregarCaches();
  const turmas = window._cacheTurmas;
  const alunos = window._cacheAlunos;

  _popularSelect(document.getElementById('vincular-aluno'), alunos, 'id',
    a => `${a.nome} (#${String(a.id).padStart(3,'0')})`, 'Selecione o aluno...');
  _popularSelect(document.getElementById('vincular-turma'), turmas, 'id',
    t => `${t.nome} (${t.total_alunos}/${t.capacidade_max} vagas) ${t.ocupacao_pct>=80?'⚠️':''}`,
    'Selecione a turma...');

  const alertaEl = document.getElementById('alerta-turmas-cheias');
  if (alertaEl) {
    alertaEl.innerHTML = turmas.filter(t => t.ocupacao_pct >= 80).map(t => `
      <div class="alert alert-info" style="margin-bottom:12px">
        ⚠️ A turma <strong>${t.nome}</strong> está com ${t.ocupacao_pct}% de ocupação (${t.total_alunos}/${t.capacidade_max} vagas).
      </div>`).join('');
  }

  const grid = document.getElementById('grid-controle-turmas');
  if (!grid) return;
  if (!turmas.length) { grid.innerHTML = '<div style="color:var(--gray-500);font-size:13px;padding:8px">Nenhuma turma cadastrada.</div>'; return; }

  grid.innerHTML = turmas.map(t => {
    const alunosTurma = alunos.filter(a => a.id_turma === t.id);
    const barColor = t.ocupacao_pct >= 80 ? 'var(--warning)' : 'var(--blue-500)';
    const badgeVagas = t.ocupacao_pct >= 80
      ? `<span class="badge badge-warning">${t.total_alunos} / ${t.capacidade_max} ⚠️</span>`
      : `<span class="badge badge-blue">${t.total_alunos} / ${t.capacidade_max}</span>`;
    return `
      <div class="card">
        <div class="card-header">
          <div>
            <div class="card-title">${t.nome}</div>
            <div style="font-size:12px;color:var(--gray-500);margin-top:3px">${_nomeProfessor(t.id_professor)} · ${t.horario}</div>
          </div>
          <div style="text-align:right">
            ${badgeVagas}
            <div style="background:var(--gray-200);border-radius:99px;height:5px;margin-top:6px;width:80px">
              <div style="background:${barColor};border-radius:99px;height:5px;width:${t.ocupacao_pct}%"></div>
            </div>
          </div>
        </div>
        <div class="card-body" style="padding:0">
          <table>
            <thead><tr><th>Aluno</th><th>Matrícula</th><th>Status</th><th></th></tr></thead>
            <tbody>
              ${!alunosTurma.length
                ? `<tr><td colspan="4" style="text-align:center;color:var(--gray-500);padding:16px">Nenhum aluno nesta turma</td></tr>`
                : alunosTurma.slice(0,4).map(a => `
                  <tr>
                    <td>${a.nome}</td>
                    <td>${a.data_matricula}</td>
                    <td>${_badge(a.status)}</td>
                    <td><button class="btn btn-danger btn-sm btn-icon" onclick="desvincularAluno(${a.id})" title="Remover da turma">✕</button></td>
                  </tr>`).join('')}
              ${alunosTurma.length > 4 ? `<tr><td colspan="4" style="text-align:center;color:var(--gray-500);font-size:12.5px;padding:10px">+ ${alunosTurma.length-4} aluno(s)</td></tr>` : ''}
            </tbody>
          </table>
        </div>
      </div>`;
  }).join('');
}

function abrirModalVincular() {
  _popularSelect(document.getElementById('vincular-aluno'), window._cacheAlunos, 'id',
    a => `${a.nome} (#${String(a.id).padStart(3,'0')})`, 'Selecione o aluno...');
  _popularSelect(document.getElementById('vincular-turma'), window._cacheTurmas, 'id',
    t => `${t.nome} (${t.total_alunos}/${t.capacidade_max} vagas) ${t.ocupacao_pct>=80?'⚠️':''}`,
    'Selecione a turma...');
  const aviso = document.getElementById('aviso-vagas');
  if (aviso) aviso.innerHTML = '';
  openModal('modal-vincular');
}

async function verificarVagasControle(sel) {
  const aviso = document.getElementById('aviso-vagas');
  if (!aviso || !sel.value || sel.value === '0') { if(aviso) aviso.innerHTML=''; return; }
  const t = (window._cacheTurmas || []).find(x => x.id === Number(sel.value));
  if (!t) return;
  if (t.vagas <= 0) aviso.innerHTML = `<div class="alert alert-danger">🚫 Turma sem vagas!</div>`;
  else if (t.ocupacao_pct >= 80) aviso.innerHTML = `<div class="alert alert-info">⚠️ ${t.ocupacao_pct}% ocupada. ${t.vagas} vaga(s).</div>`;
  else aviso.innerHTML = `<div class="alert alert-success">✅ ${t.vagas} vaga(s) disponível(is).</div>`;
}

async function vincularAluno() {
  const idAluno = Number(document.getElementById('vincular-aluno')?.value);
  const idTurma = Number(document.getElementById('vincular-turma')?.value);
  if (!idAluno || !idTurma) { showToast('Selecione aluno e turma!', 'danger'); return; }
  const aluno = await GET(`/alunos?id=${idAluno}`);
  if (!aluno) return;
  aluno.id_turma = idTurma;
  const res = await PUT(`/alunos?id=${idAluno}`, aluno);
  if (res?.ok) { closeModal('modal-vincular'); showToast('Aluno vinculado!'); await _carregarCaches(); carregarControle(); }
  else showToast('Erro ao vincular.', 'danger');
}

async function desvincularAluno(idAluno) {
  if (!confirm('Remover aluno desta turma?')) return;
  const aluno = await GET(`/alunos?id=${idAluno}`);
  if (!aluno) return;
  aluno.id_turma = 0;
  const res = await PUT(`/alunos?id=${idAluno}`, aluno);
  if (res?.ok) { showToast('Aluno removido da turma.'); await _carregarCaches(); carregarControle(); }
  else showToast('Erro ao remover.', 'danger');
}

/* ============================================================ AGENDA */
window._todasAsAulas = [];

async function carregarAgenda() {
  await _carregarCaches();
  const agenda = await GET('/agenda');
  window._todasAsAulas = agenda || [];
  const lista = window._todasAsAulas;
  const total      = lista.length;
  const realizadas = lista.filter(a => a.realizada === 1).length;
  const pendentes  = total - realizadas;
  const setEl = (id, val) => { const e=document.getElementById(id); if(e) e.textContent=val; };
  setEl('stat-total-aulas', total);
  setEl('stat-realizadas',  realizadas);
  setEl('stat-pendentes',   pendentes);
  setEl('total-agenda',     `${total} aula(s)`);

  const selProf = document.getElementById('filtro-agenda-prof');
  if (selProf) {
    selProf.innerHTML = '<option value="">Todos os professores</option>' +
      (window._cacheProfessores || []).map(p => `<option value="${p.id}">${p.nome}</option>`).join('');
  }

  filtrarAgenda();
}

function filtrarAgenda() {
  const busca   = (document.getElementById('busca-agenda')?.value || '').toLowerCase();
  const profId  = document.getElementById('filtro-agenda-prof')?.value || '';
  const status  = document.getElementById('filtro-agenda-status')?.value || '';

  const filtradas = (window._todasAsAulas || []).filter(ag => {
    const textoBusca = `${ag.atividade} ${_nomeProfessor(ag.id_professor)} ${_nomeTurma(ag.id_turma)}`.toLowerCase();
    const okBusca = !busca || textoBusca.includes(busca);
    const okProf = !profId || String(ag.id_professor) === String(profId);
    const okStatus = !status || String(ag.realizada) === String(status);
    return okBusca && okProf && okStatus;
  });

  const tbody = document.getElementById('lista-agenda');
  if (tbody) {
    if (!filtradas.length) { _vazio('lista-agenda', 9, 'Nenhuma aula encontrada com os filtros selecionados.'); }
    else {
      tbody.innerHTML = filtradas.map(ag => `
        <tr>
          <td><strong>AG${String(ag.id).padStart(3,'0')}</strong></td>
          <td>${ag.atividade}</td>
          <td>${_nomeProfessor(ag.id_professor)}</td>
          <td>${_nomeTurma(ag.id_turma)}</td>
          <td>${ag.data}</td>
          <td>${ag.hora_inicio}</td>
          <td>${ag.hora_fim || '—'}</td>
          <td>${ag.realizada ? '<span class="badge badge-success">Realizada</span>' : '<span class="badge badge-warning">Pendente</span>'}</td>
          <td class="td-actions">
            <button class="btn btn-outline btn-sm btn-icon" onclick="editarAula(${ag.id})">✏️</button>
            ${!ag.realizada ? `<button class="btn btn-success btn-sm btn-icon" onclick="marcarRealizada(${ag.id})">✅</button>` : ''}
            <button class="btn btn-danger btn-sm btn-icon" onclick="excluirAula(${ag.id})">🗑️</button>
          </td>
        </tr>`).join('');
    }
  }

  const grid = document.getElementById('grid-agenda');
  if (grid) {
    if (!filtradas.length) { grid.innerHTML = '<div style="color:var(--gray-500);font-size:13px;padding:8px">Nenhuma aula encontrada.</div>'; }
    else {
      grid.innerHTML = filtradas.map(ag => `
        <div class="agenda-item" style="border-left-color:${ag.realizada?'var(--success)':'var(--blue-500)'}">
          <div class="agenda-time">${ag.hora_inicio}${ag.hora_fim?' – '+ag.hora_fim:''}</div>
          <div class="agenda-prof">👤 ${_nomeProfessor(ag.id_professor)}</div>
          <div class="agenda-ativ">📋 ${ag.atividade}</div>
          <div class="agenda-turma">
            <span class="badge badge-blue">${_nomeTurma(ag.id_turma)}</span>
            <span style="margin-left:6px;font-size:12px;color:var(--gray-500)">${ag.data}</span>
          </div>
          ${ag.observacao ? `<div style="font-size:12px;color:var(--gray-500);margin-top:6px">💬 ${ag.observacao}</div>` : ''}
          <div style="display:flex;gap:6px;margin-top:10px">
            ${!ag.realizada
              ? `<button class="btn btn-success btn-sm" style="flex:1" onclick="marcarRealizada(${ag.id})">✅ Realizada</button>`
              : `<span class="badge badge-success" style="padding:6px 12px">✅ Concluída</span>`}
            <button class="btn btn-danger btn-sm btn-icon" onclick="excluirAula(${ag.id})">🗑️</button>
          </div>
        </div>`).join('');
    }
  }
}

async function abrirModalNovaAula() {
  await _carregarCaches();
  document.getElementById('aula-id').value        = '';
  document.getElementById('aula-atividade').value = '';
  document.getElementById('aula-data').value      = '';
  document.getElementById('aula-inicio').value    = '';
  document.getElementById('aula-fim').value       = '';
  document.getElementById('aula-obs').value       = '';
  document.getElementById('aula-realizada').value = '0';
  _popularSelect(document.getElementById('aula-professor'), window._cacheProfessores, 'id', p => p.nome, 'Selecionar professor...');
  _popularSelect(document.getElementById('aula-turma'),     window._cacheTurmas,      'id', t => t.nome, 'Selecionar turma...');
  const titulo = document.getElementById('modal-aula-titulo');
  if (titulo) titulo.textContent = '➕ Agendar Nova Aula';
  openModal('modal-aula');
}

async function editarAula(id) {
  await _carregarCaches();
  const agenda = await GET('/agenda');
  const ag = (agenda || []).find(x => x.id === id);
  if (!ag) return;
  document.getElementById('aula-id').value        = ag.id;
  document.getElementById('aula-atividade').value = ag.atividade   || '';
  document.getElementById('aula-data').value      = ag.data        || '';
  document.getElementById('aula-inicio').value    = ag.hora_inicio || '';
  document.getElementById('aula-fim').value       = ag.hora_fim    || '';
  document.getElementById('aula-obs').value       = ag.observacao  || '';
  document.getElementById('aula-realizada').value = ag.realizada   || 0;
  _popularSelect(document.getElementById('aula-professor'), window._cacheProfessores, 'id', p => p.nome, 'Selecionar professor...');
  _popularSelect(document.getElementById('aula-turma'),     window._cacheTurmas,      'id', t => t.nome, 'Selecionar turma...');
  const selP = document.getElementById('aula-professor'); if(selP) selP.value = ag.id_professor || 0;
  const selT = document.getElementById('aula-turma');     if(selT) selT.value = ag.id_turma     || 0;
  const titulo = document.getElementById('modal-aula-titulo');
  if (titulo) titulo.textContent = '✏️ Editar Aula';
  openModal('modal-aula');
}

async function salvarAula() {
  const atividade = document.getElementById('aula-atividade')?.value.trim();
  const data      = document.getElementById('aula-data')?.value.trim();
  const inicio    = document.getElementById('aula-inicio')?.value.trim();
  if (!atividade) { showToast('Informe a atividade!',        'danger'); return; }
  if (!data)      { showToast('Informe a data!',             'danger'); return; }
  if (!inicio)    { showToast('Informe o horário de início!','danger'); return; }
  const body = {
    atividade, data,
    hora_inicio:  inicio,
    hora_fim:     document.getElementById('aula-fim')?.value.trim()        || '',
    observacao:   document.getElementById('aula-obs')?.value.trim()        || '',
    id_professor: Number(document.getElementById('aula-professor')?.value) || 0,
    id_turma:     Number(document.getElementById('aula-turma')?.value)     || 0,
    realizada:    Number(document.getElementById('aula-realizada')?.value) || 0,
  };
  const idRaw = document.getElementById('aula-id')?.value;
  const id    = idRaw ? Number(idRaw) : null;
  const res   = id ? await PUT(`/agenda?id=${id}`, body) : await POST('/agenda', body);
  if (res?.ok) { closeModal('modal-aula'); showToast(id ? 'Aula atualizada!' : 'Aula agendada!'); carregarAgenda(); }
  else showToast('Erro ao salvar aula.', 'danger');
}

async function marcarRealizada(id) {
  const res = await PUT(`/agenda?id=${id}`, { realizada: 1 });
  if (res?.ok) { showToast('Aula marcada como realizada!'); carregarAgenda(); }
  else showToast('Erro ao atualizar.', 'danger');
}

async function excluirAula(id) {
  if (!confirm('Deseja excluir este agendamento?')) return;
  const res = await DEL(`/agenda?id=${id}`);
  if (res?.ok) { showToast('Agendamento removido.'); carregarAgenda(); }
  else showToast('Erro ao remover.', 'danger');
}

/* ============================================================ AUTO-INIT */
document.addEventListener('DOMContentLoaded', async () => {
  await _carregarCaches();
});