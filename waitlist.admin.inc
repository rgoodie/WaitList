<?php

/**
 * @file
 * TODO: Enter file description here.
 */

/**
 * Form builder.
 */
function waitlist_settings_form($form, &$form_state)
{

    // Run wait list
    WaitList::moveNamesIntoWaitList();

    $form[] = [
        '#markup' => t("<h2>Add people to waitlist</h2>"),
    ];

    foreach (WaitList::getAddedUserRoles() as $role) {


        $role_with_underscore = str_replace(' ', '_', $role);

        $form['waitlist_for_' . $role_with_underscore] = [
            '#type' => 'textarea',
            '#title' => t(ucwords($role)),
            '#description' => t("One username per line"),
            '#rows' => 3,
            '#default_value' => variable_get('waitlist_for_' . $role_with_underscore, '')
        ];

    }




    $individual_usernames = WaitList::getUniqueIndividualUsers();

    if (!empty($individual_usernames)) {

        $form[] = ['#markup' => t(
            "<h2>People to be added on their next login or initial account creation</h2> "
        )];
        foreach ($individual_usernames as $username) {
            $form[] = array(
                '#markup' => theme_item_list([
                    'title' => sprintf(
                        "%s [ %s ]",
                        $username ,
                        l('delete request', "admin/people/permissions/waitlist/$username" )
                    ),
                    'items' => WaitList::getRolesforUserOnWaitList($username),
                    'type' => 'ul',
                    'attributes' => []
                ])
            );
        }


    }


    return system_settings_form($form);
}
