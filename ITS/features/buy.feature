Feature: Buying goods

Scenario: Adding a product to the shopping cart (6)
    Given User is on the product page
    When User clicks on the "Add to cart" button
    Then Success message is displayed
    And Product is added to the shopping cart

Scenario: Adding a product to the shopping cart multiple times (7)
    Given User is on the product page
    When User clicks on the "Add to cart" button multiple times
    Then Product quantity in the shopping cart matches the number of times the button was pressed

Scenario: Updating product quantity in the shopping cart (8)
    Given User is on the shopping cart page
    And Product is already added to the shopping cart
    When User updates the product quantity to a specific number
    And User clicks on the "Update" button
    Then Product quantity is updated in the shopping cart

Scenario: Removing a product from the shopping cart (10)
    Given User is on the shopping cart page
    And Product is in the shopping cart
    When User clicks on the "Remove" button
    Then Product is removed from the shopping cart

Scenario: Adding 0 quantity of a product to the shopping cart (9)
    Given User is on the product page
    But Quantity is set to 0
    And User clicks on the "Add to cart" button
    Then Product is not added to the shopping cart

Scenario: Checking out from the shopping cart (11)
    Given User has added products to the shopping cart
    And User is on the shopping cart page
    When User clicks on the "Checkout" button on the shopping cart page
    Then User is taken to the checkout page to complete their purchase