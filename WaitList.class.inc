<?php

/**
 * Created by PhpStorm.
 * Helper class that does the heavy lifting. Lots of static classes helps separate out
 * layers of processing.
 */
class WaitList
{
    /**
     * Gets sorted, unique usernames currently in the waitlist table
     * @return array|bool|DatabaseStatementInterface
     */
    static function getUniqueIndividualUsers()
    {

        $ind_usernames = db_query("select distinct username from {waitlist} order by username;");
        $ind_usernames = $ind_usernames->fetchCol(0);
        $ind_usernames = array_unique($ind_usernames);
        sort($ind_usernames);

        // if empty return false
        if (empty($ind_usernames)) {
            return false;
        }

        return $ind_usernames;

    }

    /**
     * Adds username into the waitlist table.
     */
    static function moveNamesIntoWaitList()
    {

        $roles = self::getAddedUserRoles();

        foreach ($roles as $role) {
            $var_name = 'waitlist_for_' . str_replace(' ', '_', $role);
            $var_contents = variable_get($var_name, '');
            $var_contents = rtrim($var_contents, PHP_EOL);
            $var_contents = explode(PHP_EOL, $var_contents);

            // skip if first empty
            if ($var_contents[0] == '') {
                continue;
            }

            // watchdog('waitlist', print_r($var_contents, TRUE));

            // put each user into list
            foreach ($var_contents as $username) {
                self::insertRecord($username, $role);
            }

            // clean out list once we've moved it to the holding table
            variable_del($var_name);
        }
    }

    /**
     * Returns of array of all Roles. It omits the anonymous and authenticated roles preset in Drupal.
     * @return array
     */
    static function getAddedUserRoles()
    {
        $out_roles = array();

        foreach (user_roles() as $role) {
            // skip standard drupal roles.
            $role = trim($role);
            if (array_search($role, ['anonymous user', 'authenticated user', 'administrator']) !== FALSE) {
                continue;
            }

            $out_roles[] = $role;
        }

        return $out_roles;
    }

    static function insertRecord($username, $role)
    {

        $username = trim(filter_var($username, FILTER_SANITIZE_STRING));
        $role = trim(filter_var($role, FILTER_SANITIZE_STRING));

        // Does this already exist
        $count = db_query(
            "select count( username) from waitlist where role=:role and username=:user;", [
            ':role' => $role,
            ':user' => $username
        ])->fetchField(0);


        if ($count > 0) {
            watchdog('waitlist', "$username already wait-listed for $role.");
            return false;
        }

        $insert_restult = db_insert('waitlist')
            ->fields(array(
                'role' => $role,
                'username' => $username,
            ))
            ->execute();


        if ($insert_restult) {
            watchdog('waitlist', t("Inserted @user queued for @role"), array(
                '@user' => $username,
                '@role' => $role
            ));
            return true;
        } else {
            watchdog('waitlist', t("FAILED TO INSERT @user queued for @role"), array(
                '@user' => $username,
                '@role' => $role
            ));
            return false;
        }
    }

    static function deleteRequest($username)
    {

        $username = filter_var($username, FILTER_SANITIZE_STRING);
        $roles = implode(', ', self::getRolesforUserOnWaitList($username));

        $result = db_delete('waitlist')
            ->condition('username', $username)
            ->execute();

        if ($result) {
            $msg = sprintf("Request for %s was removed. Those roles were %s", $username, $roles);
            $flag = TRUE;
        } else {
            $msg = sprintf("Waitlist for %s was cleared. Previous request included roles: %s", $username, $roles);
            $flag = FALSE;
        }

        watchdog('waitlist', $msg);
        dpm($msg);


    }

    /**
     * Returns all roles for a single wait-listed username
     * @param $username
     * @return bool
     */
    static function getRolesforUserOnWaitList($username)
    {
        $roles_array = db_query("select distinct role from {waitlist} where username=:u", [':u' => $username])->fetchCol(0);

        // if empty return false
        if (empty($roles_array)) {
            return false;
        }

        return $roles_array;

    }

    static function addRoleToExistingAccount($user_uid, $role)
    {
        $u = module_invoke('user', 'load', $user_uid);
        $user_roles = $u->roles;
        $user_roles[] = 'newrole';
        module_invoke('user', 'save', $u, [
            'roles' => $user_roles
        ]);

        watchdog('waitlist', t("@role has been attached to @user (@uid)", [
            '@role' => $role,
            '@user' => $u->name,
            '@uid' => $u->uid,
        ]));
    }
}

