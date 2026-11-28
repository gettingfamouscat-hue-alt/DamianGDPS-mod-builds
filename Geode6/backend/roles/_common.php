<?php
function roleJSON(array $body, int $status = 200): void {
    http_response_code($status);
    header('Content-Type: application/json; charset=utf-8');
    header('Cache-Control: no-store');
    echo json_encode($body);
    exit;
}
function roleRequest(): array {
    if (($_SERVER['REQUEST_METHOD'] ?? '') !== 'POST') roleJSON(['ok' => false, 'error' => 'POST required.'], 405);
    chdir(__DIR__);
    require_once '../incl/lib/generatePass.php';
    require_once '../incl/lib/RoleManager.php';
    include '../incl/lib/connection.php';
    // Compatibility for the initial client, which sent POST query parameters.
    // Both forms still require a valid account credential; cookies grant nothing.
    $input = $_POST + $_GET;
    $id = $input['accountID'] ?? '';
    $credential = $input['gjp2'] ?? '';
    if (!is_string($id) || !ctype_digit($id) || (int)$id <= 0 || (int)$id > 2147483647
        || !is_string($credential) || GeneratePass::isGJP2Valid((int)$id, $credential) !== 1)
        roleJSON(['ok' => false, 'error' => 'Authentication required.'], 401);
    $id = (int)$id;
    return [new RoleManager($db), $id, $db];
}
