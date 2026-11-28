<?php
require_once __DIR__ . '/_common.php';
[$roles, $accountID] = roleRequest();
roleJSON(array_merge(['ok' => true], $roles->access($accountID)));
