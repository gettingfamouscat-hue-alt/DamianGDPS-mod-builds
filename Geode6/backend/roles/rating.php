<?php
require_once __DIR__ . '/_common.php';
try {
    [$roles, $accountID, $db] = roleRequest();
    require_once '../incl/lib/StaffRating.php';
    roleJSON((new StaffRating($db, $roles, new mainLib()))->handle($accountID, $_POST));
} catch (DomainException $e) {
    roleJSON(['ok' => false, 'error' => $e->getMessage()], in_array($e->getCode(), [400, 403, 404], true) ? $e->getCode() : 400);
} catch (Throwable $e) {
    error_log('Staff rating failed: ' . $e->getMessage());
    roleJSON(['ok' => false, 'error' => 'The rating service is unavailable. Try again shortly.'], 503);
}
