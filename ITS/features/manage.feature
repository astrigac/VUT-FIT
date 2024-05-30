Feature: Managing goods and their availability

Scenario: Viewing list of products (12)
    Given User is on the admin dashboard
    And User is logged in as an admin
    When User clicks on the "Products" tab in the drop down menu
    Then List of products with details is displayed
