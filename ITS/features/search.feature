Feature: Searching goods

Scenario: Searching for an existing product (1)
    Given User is on the home page
    When User types a query into the search
    And User presses search
    Then Results page is displayed with relevant product

Scenario: Searching for a product with no results (2)
    Given User is on the home page
    When User types a query with non existent product into the search
    And User presses search
    Then A message is displayed indicating no products were found

Scenario: Searching products by category (3)
    Given User is on the home page
    When User clicks on a desired category
    Then Results page is displayed with relevant products

Scenario: Opening a product page from results page (4)
    Given User is on the results page
    When User clicks on a product
    Then Corresponding product page is displayed

Scenario: Sorting products within a category by different criteria (5)
    Given User is on a category page displaying multiple products
    When User selects an option from the sorting dropdown
    Then Products are reorganized and displayed according to the expected order
