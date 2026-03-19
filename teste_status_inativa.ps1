# Teste de Status Inativa em Turmas
Write-Host "=== TESTE: Status Inativa em Turmas ===" -ForegroundColor Green

# Aguarda o servidor (já deve estar rodando)
Start-Sleep -Seconds 1

$apiBase = "http://localhost:8080/api"

# Teste 1: Criar uma turma inativa
Write-Host "`n1. Criando turma INATIVA..." -ForegroundColor Yellow
$turmaInativa = @{
    nome = "Pilates D (INATIVA)"
    modalidade = "Pilates"
    id_professor = 1
    horario = "18:00"
    dias_semana = "Ter-Qui-Sab"
    capacidade_max = 12
    status = 0
} | ConvertTo-Json

$resp = Invoke-WebRequest -Uri "$apiBase/turmas" -Method Post -Body $turmaInativa -ContentType "application/json" -ErrorAction SilentlyContinue
if ($resp.StatusCode -eq 201) {
    $turmaId = ($resp.Content | ConvertFrom-Json).id
    Write-Host "✓ Turma inativa criada com ID: $turmaId" -ForegroundColor Green
    
    # Teste 2: Verificar se a turma retorna com status "Inativo"
    Write-Host "`n2. Verificando se turma retorna com status 'Inativo'..." -ForegroundColor Yellow
    $turmas = Invoke-WebRequest -Uri "$apiBase/turmas" -Method Get -ErrorAction SilentlyContinue
    $turmaList = $turmas.Content | ConvertFrom-Json
    $turmaTest = $turmaList | Where-Object { $_.id -eq $turmaId }
    
    if ($turmaTest.status -eq "Inativo") {
        Write-Host "✓ Status correto: $($turmaTest.status)" -ForegroundColor Green
    } else {
        Write-Host "✗ Status incorreto: $($turmaTest.status) (esperado: Inativo)" -ForegroundColor Red
    }
    
    # Teste 3: Editar a turma para ativa
    Write-Host "`n3. Editando turma para ATIVA..." -ForegroundColor Yellow
    $turmaAtiva = @{
        nome = "Pilates D (ATIVA)"
        modalidade = "Pilates"
        id_professor = 1
        horario = "18:00"
        dias_semana = "Ter-Qui-Sab"
        capacidade_max = 12
        status = 1
    } | ConvertTo-Json
    
    $resp2 = Invoke-WebRequest -Uri "$apiBase/turmas?id=$turmaId" -Method Put -Body $turmaAtiva -ContentType "application/json" -ErrorAction SilentlyContinue
    if ($resp2.StatusCode -eq 200) {
        Write-Host "✓ Turma editada" -ForegroundColor Green
        
        # Teste 4: Verificar se status foi atualizado
        Write-Host "`n4. Verificando novo status..." -ForegroundColor Yellow
        $turmas2 = Invoke-WebRequest -Uri "$apiBase/turmas" -Method Get -ErrorAction SilentlyContinue
        $turmaList2 = $turmas2.Content | ConvertFrom-Json
        $turmaTest2 = $turmaList2 | Where-Object { $_.id -eq $turmaId }
        
        if ($turmaTest2.status -eq "Ativo") {
            Write-Host "✓ Status atualizado corretamente para: $($turmaTest2.status)" -ForegroundColor Green
        } else {
            Write-Host "✗ Status não foi atualizado: $($turmaTest2.status)" -ForegroundColor Red
        }
    }
} else {
    Write-Host "✗ Erro ao criar turma" -ForegroundColor Red
}

Write-Host "`n=== FIM DO TESTE ===" -ForegroundColor Green
