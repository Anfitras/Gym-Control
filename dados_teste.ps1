$h = @{"Content-Type"="application/json; charset=utf-8"}

function Post($url, $body) {
    $bytes = [System.Text.Encoding]::UTF8.GetBytes($body)
    $req = [System.Net.WebRequest]::Create($url)
    $req.Method = "POST"
    $req.ContentType = "application/json"
    $req.ContentLength = $bytes.Length
    $stream = $req.GetRequestStream()
    $stream.Write($bytes, 0, $bytes.Length)
    $stream.Close()
    try { $req.GetResponse() | Out-Null } catch {}
    Start-Sleep -Milliseconds 200
}

Write-Host "Cadastrando professores..."
Post "http://localhost:8080/api/professores" '{"nome":"Carlos Mendes","cpf":"111.222.333-44","especialidade":"Musculacao","telefone":"(35) 98801-1111","email":"carlos@gymcontrol.com","horario_disponivel":"Seg-Sex 06:00-18:00","status":1}'
Post "http://localhost:8080/api/professores" '{"nome":"Ana Beatriz","cpf":"222.333.444-55","especialidade":"Funcional","telefone":"(35) 99234-2222","email":"ana@gymcontrol.com","horario_disponivel":"Seg-Sab 07:00-17:00","status":1}'
Post "http://localhost:8080/api/professores" '{"nome":"Ricardo Lima","cpf":"333.444.555-66","especialidade":"Spinning","telefone":"(35) 97456-3333","email":"ricardo@gymcontrol.com","horario_disponivel":"Ter-Qui-Sab 08:00-14:00","status":1}'
Post "http://localhost:8080/api/professores" '{"nome":"Fernanda Reis","cpf":"444.555.666-77","especialidade":"Zumba","telefone":"(35) 98765-4444","email":"fernanda@gymcontrol.com","horario_disponivel":"Seg-Qua-Sex 09:00-15:00","status":1}'
Post "http://localhost:8080/api/professores" '{"nome":"Marcos Vieira","cpf":"555.666.777-88","especialidade":"Avaliacao Fisica","telefone":"(35) 99888-5555","email":"marcos@gymcontrol.com","horario_disponivel":"Seg-Sex 08:00-12:00","status":1}'

Write-Host "Cadastrando turmas..."
Post "http://localhost:8080/api/turmas" '{"nome":"Musculacao A","modalidade":"Musculacao","id_professor":1,"horario":"07:00","dias_semana":"Seg-Qua-Sex","capacidade_max":15,"status":1}'
Post "http://localhost:8080/api/turmas" '{"nome":"Funcional B","modalidade":"Funcional","id_professor":2,"horario":"08:00","dias_semana":"Ter-Qui-Sab","capacidade_max":10,"status":1}'
Post "http://localhost:8080/api/turmas" '{"nome":"Spinning C","modalidade":"Spinning","id_professor":3,"horario":"09:00","dias_semana":"Ter-Qui-Sab","capacidade_max":10,"status":1}'
Post "http://localhost:8080/api/turmas" '{"nome":"Zumba A","modalidade":"Zumba","id_professor":4,"horario":"10:00","dias_semana":"Seg-Qua-Sex","capacidade_max":15,"status":1}'

Write-Host "Cadastrando alunos..."
Post "http://localhost:8080/api/alunos" '{"nome":"Lucas Ferreira","cpf":"001.001.001-01","idade":22,"sexo":1,"telefone":"(35) 98801-0001","email":"lucas@email.com","id_turma":1,"id_professor":1,"status":1,"peso_kg":78.0,"altura_m":1.78}'
Post "http://localhost:8080/api/alunos" '{"nome":"Mariana Costa","cpf":"002.002.002-02","idade":25,"sexo":2,"telefone":"(35) 98801-0002","email":"mariana@email.com","id_turma":2,"id_professor":2,"status":1,"peso_kg":62.0,"altura_m":1.65}'
Post "http://localhost:8080/api/alunos" '{"nome":"Pedro Alves","cpf":"003.003.003-03","idade":30,"sexo":1,"telefone":"(35) 98801-0003","email":"pedro@email.com","id_turma":3,"id_professor":3,"status":1,"peso_kg":85.0,"altura_m":1.80}'
Post "http://localhost:8080/api/alunos" '{"nome":"Juliana Matos","cpf":"004.004.004-04","idade":28,"sexo":2,"telefone":"(35) 98801-0004","email":"juliana@email.com","id_turma":4,"id_professor":4,"status":0,"peso_kg":58.0,"altura_m":1.62}'
Post "http://localhost:8080/api/alunos" '{"nome":"Rafael Souza","cpf":"005.005.005-05","idade":20,"sexo":1,"telefone":"(35) 98801-0005","email":"rafael@email.com","id_turma":1,"id_professor":1,"status":1,"peso_kg":72.0,"altura_m":1.75}'
Post "http://localhost:8080/api/alunos" '{"nome":"Camila Torres","cpf":"006.006.006-06","idade":23,"sexo":2,"telefone":"(35) 98801-0006","email":"camila@email.com","id_turma":2,"id_professor":2,"status":1,"peso_kg":55.0,"altura_m":1.60}'
Post "http://localhost:8080/api/alunos" '{"nome":"Thiago Barbosa","cpf":"007.007.007-07","idade":35,"sexo":1,"telefone":"(35) 98801-0007","email":"thiago@email.com","id_turma":1,"id_professor":1,"status":1,"peso_kg":90.0,"altura_m":1.82}'
Post "http://localhost:8080/api/alunos" '{"nome":"Vanessa Lima","cpf":"008.008.008-08","idade":27,"sexo":2,"telefone":"(35) 98801-0008","email":"vanessa@email.com","id_turma":4,"id_professor":4,"status":2,"peso_kg":65.0,"altura_m":1.68}'
Post "http://localhost:8080/api/alunos" '{"nome":"Bruno Castro","cpf":"009.009.009-09","idade":19,"sexo":1,"telefone":"(35) 98801-0009","email":"bruno@email.com","id_turma":1,"id_professor":1,"status":1,"peso_kg":68.0,"altura_m":1.72}'
Post "http://localhost:8080/api/alunos" '{"nome":"Patricia Nunes","cpf":"010.010.010-10","idade":31,"sexo":2,"telefone":"(35) 98801-0010","email":"patricia@email.com","id_turma":2,"id_professor":2,"status":1,"peso_kg":60.0,"altura_m":1.63}'
Post "http://localhost:8080/api/alunos" '{"nome":"Diego Martins","cpf":"011.011.011-11","idade":24,"sexo":1,"telefone":"(35) 98801-0011","email":"diego@email.com","id_turma":3,"id_professor":3,"status":1,"peso_kg":80.0,"altura_m":1.77}'
Post "http://localhost:8080/api/alunos" '{"nome":"Fernanda Gomes","cpf":"012.012.012-12","idade":29,"sexo":2,"telefone":"(35) 98801-0012","email":"fgomes@email.com","id_turma":4,"id_professor":4,"status":1,"peso_kg":57.0,"altura_m":1.61}'
Post "http://localhost:8080/api/alunos" '{"nome":"Henrique Lopes","cpf":"013.013.013-13","idade":26,"sexo":1,"telefone":"(35) 98801-0013","email":"henrique@email.com","id_turma":1,"id_professor":1,"status":1,"peso_kg":75.0,"altura_m":1.79}'
Post "http://localhost:8080/api/alunos" '{"nome":"Aline Carvalho","cpf":"014.014.014-14","idade":21,"sexo":2,"telefone":"(35) 98801-0014","email":"aline@email.com","id_turma":2,"id_professor":2,"status":1,"peso_kg":52.0,"altura_m":1.58}'
Post "http://localhost:8080/api/alunos" '{"nome":"Gustavo Rocha","cpf":"015.015.015-15","idade":33,"sexo":1,"telefone":"(35) 98801-0015","email":"gustavo@email.com","id_turma":3,"id_professor":3,"status":1,"peso_kg":88.0,"altura_m":1.83}'
Post "http://localhost:8080/api/alunos" '{"nome":"Isabela Dias","cpf":"016.016.016-16","idade":22,"sexo":2,"telefone":"(35) 98801-0016","email":"isabela@email.com","id_turma":4,"id_professor":4,"status":1,"peso_kg":54.0,"altura_m":1.64}'
Post "http://localhost:8080/api/alunos" '{"nome":"Rodrigo Pinto","cpf":"017.017.017-17","idade":28,"sexo":1,"telefone":"(35) 98801-0017","email":"rodrigo@email.com","id_turma":1,"id_professor":1,"status":1,"peso_kg":82.0,"altura_m":1.76}'
Post "http://localhost:8080/api/alunos" '{"nome":"Leticia Moura","cpf":"018.018.018-18","idade":24,"sexo":2,"telefone":"(35) 98801-0018","email":"leticia@email.com","id_turma":2,"id_professor":2,"status":0,"peso_kg":59.0,"altura_m":1.66}'
Post "http://localhost:8080/api/alunos" '{"nome":"Felipe Azevedo","cpf":"019.019.019-19","idade":27,"sexo":1,"telefone":"(35) 98801-0019","email":"felipe@email.com","id_turma":3,"id_professor":3,"status":1,"peso_kg":77.0,"altura_m":1.74}'
Post "http://localhost:8080/api/alunos" '{"nome":"Tatiane Ribeiro","cpf":"020.020.020-20","idade":32,"sexo":2,"telefone":"(35) 98801-0020","email":"tatiane@email.com","id_turma":4,"id_professor":4,"status":1,"peso_kg":63.0,"altura_m":1.67}'
Post "http://localhost:8080/api/alunos" '{"nome":"Anderson Silva","cpf":"021.021.021-21","idade":25,"sexo":1,"telefone":"(35) 98801-0021","email":"anderson@email.com","id_turma":1,"id_professor":1,"status":1,"peso_kg":79.0,"altura_m":1.81}'
Post "http://localhost:8080/api/alunos" '{"nome":"Priscila Fonseca","cpf":"022.022.022-22","idade":26,"sexo":2,"telefone":"(35) 98801-0022","email":"priscila@email.com","id_turma":2,"id_professor":2,"status":1,"peso_kg":56.0,"altura_m":1.59}'
Post "http://localhost:8080/api/alunos" '{"nome":"Leonardo Campos","cpf":"023.023.023-23","idade":23,"sexo":1,"telefone":"(35) 98801-0023","email":"leonardo@email.com","id_turma":3,"id_professor":3,"status":1,"peso_kg":73.0,"altura_m":1.76}'
Post "http://localhost:8080/api/alunos" '{"nome":"Sabrina Teixeira","cpf":"024.024.024-24","idade":29,"sexo":2,"telefone":"(35) 98801-0024","email":"sabrina@email.com","id_turma":4,"id_professor":4,"status":2,"peso_kg":61.0,"altura_m":1.63}'
Post "http://localhost:8080/api/alunos" '{"nome":"Vinicius Almeida","cpf":"025.025.025-25","idade":20,"sexo":1,"telefone":"(35) 98801-0025","email":"vinicius@email.com","id_turma":1,"id_professor":1,"status":1,"peso_kg":69.0,"altura_m":1.73}'
Post "http://localhost:8080/api/alunos" '{"nome":"Bianca Oliveira","cpf":"026.026.026-26","idade":22,"sexo":2,"telefone":"(35) 98801-0026","email":"bianca@email.com","id_turma":2,"id_professor":2,"status":1,"peso_kg":53.0,"altura_m":1.60}'
Post "http://localhost:8080/api/alunos" '{"nome":"Eduardo Nascimento","cpf":"027.027.027-27","idade":36,"sexo":1,"telefone":"(35) 98801-0027","email":"eduardo@email.com","id_turma":3,"id_professor":3,"status":1,"peso_kg":92.0,"altura_m":1.85}'
Post "http://localhost:8080/api/alunos" '{"nome":"Monique Pereira","cpf":"028.028.028-28","idade":27,"sexo":2,"telefone":"(35) 98801-0028","email":"monique@email.com","id_turma":4,"id_professor":4,"status":1,"peso_kg":58.0,"altura_m":1.65}'
Post "http://localhost:8080/api/alunos" '{"nome":"Gabriel Sousa","cpf":"029.029.029-29","idade":21,"sexo":1,"telefone":"(35) 98801-0029","email":"gabriel@email.com","id_turma":1,"id_professor":1,"status":1,"peso_kg":71.0,"altura_m":1.77}'
Post "http://localhost:8080/api/alunos" '{"nome":"Carla Mendonca","cpf":"030.030.030-30","idade":34,"sexo":2,"telefone":"(35) 98801-0030","email":"carla@email.com","id_turma":2,"id_professor":2,"status":1,"peso_kg":66.0,"altura_m":1.69}'

Write-Host "Cadastrando agenda..."
Post "http://localhost:8080/api/agenda" '{"atividade":"Musculacao","id_professor":1,"id_turma":1,"data":"15/03/2026","hora_inicio":"07:00","hora_fim":"08:00","observacao":"Treino de forca","realizada":0}'
Post "http://localhost:8080/api/agenda" '{"atividade":"Funcional","id_professor":2,"id_turma":2,"data":"15/03/2026","hora_inicio":"08:00","hora_fim":"09:00","observacao":"Circuito funcional","realizada":0}'
Post "http://localhost:8080/api/agenda" '{"atividade":"Spinning","id_professor":3,"id_turma":3,"data":"15/03/2026","hora_inicio":"09:00","hora_fim":"10:00","observacao":"Aula de bike","realizada":1}'
Post "http://localhost:8080/api/agenda" '{"atividade":"Zumba","id_professor":4,"id_turma":4,"data":"16/03/2026","hora_inicio":"10:00","hora_fim":"11:00","observacao":"Aula de danca","realizada":0}'
Post "http://localhost:8080/api/agenda" '{"atividade":"Avaliacao Fisica","id_professor":5,"id_turma":1,"data":"16/03/2026","hora_inicio":"08:00","hora_fim":"09:00","observacao":"Avaliacao mensal","realizada":0}'
Post "http://localhost:8080/api/agenda" '{"atividade":"Musculacao","id_professor":1,"id_turma":1,"data":"17/03/2026","hora_inicio":"07:00","hora_fim":"08:00","observacao":"Treino de hipertrofia","realizada":0}'

Write-Host ""
Write-Host "Tudo cadastrado com sucesso!"
