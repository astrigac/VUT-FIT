import time
from behave import *
from selenium.webdriver.common.by import By
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.common.keys import Keys

@given(u'User is on the product page')
def step_impl(context):
    context.driver.get("http://opencart:8080/")
    context.driver.set_window_size(1440, 786)
    context.driver.find_element(By.LINK_TEXT, "Tablets").click()
    context.driver.find_element(By.LINK_TEXT, "Samsung Galaxy Tab 10.1").click()
    time.sleep(1)


@when(u'User clicks on the "Add to cart" button')
def step_impl(context):
    context.driver.find_element(By.ID, "button-cart").click()
    time.sleep(1)


@then(u'Product is added to the shopping cart')
def step_impl(context):
    # Wait for the popup to go away
    time.sleep(5)

    context.driver.find_element(By.CSS_SELECTOR, ".btn-inverse").click()
    context.driver.find_element(By.CSS_SELECTOR, ".text-end > a:nth-child(1) > strong").click()
    
    # Wait and check if the product name "Samsung Galaxy Tab 10.1" is in the cart
    assert WebDriverWait(context.driver, 10).until(
        EC.presence_of_element_located((By.XPATH, "//a[contains(@href, 'samsung-galaxy-tab-10-1') and contains(text(), 'Samsung Galaxy Tab 10.1')]"))
    )


@then(u'Success message is displayed')
def step_impl(context):
    wait = WebDriverWait(context.driver, 10) 

    # Wait until the alert container has some visible text or changes state
    alert_container = wait.until(
        EC.visibility_of_element_located((By.ID, 'alert')),
        message="The alert popup did not appear or was not visible."
    )

    assert alert_container.text != ""


@when(u'User clicks on the "Add to cart" button multiple times')
def step_impl(context):
    for i in range(2):
        context.driver.find_element(By.ID, "button-cart").click()
        time.sleep(0.5)


@then(u'Product quantity in the shopping cart matches the number of times the button was pressed')
def step_impl(context):
    # Wait for the popup to go away
    time.sleep(5)

    context.driver.find_element(By.CSS_SELECTOR, ".btn-inverse").click()
    context.driver.find_element(By.CSS_SELECTOR, ".text-end > a:nth-child(1) > strong").click()

    WebDriverWait(context.driver, 10).until(
        EC.presence_of_element_located((By.XPATH, "//a[contains(@href, 'samsung-galaxy-tab-10-1') and contains(text(), 'Samsung Galaxy Tab 10.1')]"))
    )
    # Check for quantity
    quantity = context.driver.find_element(By.XPATH, "//input[@name='quantity' and @value='3']")
    assert quantity is not None


@given(u'User is on the shopping cart page')
def step_impl(context):
    context.driver.get("http://opencart:8080/en-gb?route=checkout/cart")
    context.driver.set_window_size(1440, 786)


@given(u'Product is already added to the shopping cart')
def step_impl(context):
    # Wait and check if the product name "Samsung Galaxy Tab 10.1" is in the cart
    assert WebDriverWait(context.driver, 10).until(
        EC.presence_of_element_located((By.XPATH, "//a[contains(@href, 'samsung-galaxy-tab-10-1') and contains(text(), 'Samsung Galaxy Tab 10.1')]"))
    )


@when(u'User updates the product quantity to a specific number')
def step_impl(context):
    context.driver.find_element(By.NAME, "quantity").click()
    context.driver.find_element(By.NAME, "quantity").send_keys("7")


@when(u'User clicks on the "Update" button')
def step_impl(context):
    element = context.driver.find_element(By.CSS_SELECTOR, ".btn:nth-child(3)")
    actions = ActionChains(context.driver)
    actions.move_to_element(element).perform()
    context.driver.find_element(By.CSS_SELECTOR, ".btn:nth-child(3)").click()


@then(u'Product quantity is updated in the shopping cart')
def step_impl(context):
    wait = WebDriverWait(context.driver, 10) 

    # Wait until the alert container has some visible text or changes state
    alert_container = wait.until(
        EC.visibility_of_element_located((By.ID, 'alert')),
        message="The alert popup did not appear or was not visible."
    )
    
    quantity_input = context.driver.find_element(By.CSS_SELECTOR, "#shopping-cart .input-group input[name='quantity']")
    current_quantity = quantity_input.get_attribute('value')

    assert alert_container.text != "" and current_quantity == '37'


@given(u'User clicks on the "Add to cart" button')
def step_impl(context):
    context.driver.find_element(By.ID, "button-cart").click()


@given(u'Quantity is set to 0')
def step_impl(context):
    context.driver.find_element(By.ID, "input-quantity").click()
    context.driver.find_element(By.ID, "input-quantity").send_keys(Keys.BACKSPACE)
    time.sleep(1)
    context.driver.find_element(By.ID, "input-quantity").send_keys("0")


@then(u'Product is not added to the shopping cart')
def step_impl(context):
    context.driver.get("http://opencart:8080/en-gb?route=checkout/cart")
    element = WebDriverWait(context.driver, 20).until(
        EC.presence_of_element_located((By.XPATH, "//*[contains(., 'Your shopping cart is empty!')]")),
        message="The text 'Your shopping cart is empty!' was not found on the page."
    )
    actual_text = element.text.strip()
    expected_text = "Your shopping cart is empty!"
    assert expected_text in actual_text


@given(u'Product is in the shopping cart')
def step_impl(context):
    context.driver.find_element(By.CSS_SELECTOR, ".btn-inverse").click()
    context.driver.find_element(By.CSS_SELECTOR, ".text-end > a:nth-child(1) > strong").click()
    
    # Wait and check if the product name "Samsung Galaxy Tab 10.1" is in the cart
    assert WebDriverWait(context.driver, 10).until(
        EC.presence_of_element_located((By.XPATH, "//a[contains(@href, 'samsung-galaxy-tab-10-1') and contains(text(), 'Samsung Galaxy Tab 10.1')]"))
    )


@when(u'User clicks on the "Remove" button')
def step_impl(context):
    context.driver.find_element(By.CSS_SELECTOR, ".btn:nth-child(4) > .fa-solid").click()


@then(u'Product is removed from the shopping cart')
def step_impl(context):
    element = WebDriverWait(context.driver, 20).until(
        EC.presence_of_element_located((By.XPATH, "//*[contains(., 'Your shopping cart is empty!')]")),
        message="The text 'Your shopping cart is empty!' was not found on the page."
    )
    actual_text = element.text.strip()
    expected_text = "Your shopping cart is empty!"
    assert expected_text in actual_text


@given(u'User has added products to the shopping cart')
def step_impl(context):
    context.driver.get("http://opencart:8080/")
    context.driver.set_window_size(1440, 786)
    context.driver.find_element(By.LINK_TEXT, "Phones & PDAs").click()
    time.sleep(2)
    context.driver.execute_script("window.scrollTo(0,361)")
    time.sleep(2)
    context.driver.find_element(By.CSS_SELECTOR, ".col:nth-child(2) > .product-thumb .fa-shopping-cart").click()


@when(u'User clicks on the "Checkout" button on the shopping cart page')
def step_impl(context):
    context.driver.execute_script("window.scrollTo(0,361)")
    time.sleep(5)
    context.driver.find_element(By.CSS_SELECTOR, ".col > .btn-primary").click()


@then(u'User is taken to the checkout page to complete their purchase')
def step_impl(context):
    WebDriverWait(context.driver, 10).until(
        EC.visibility_of_element_located((By.TAG_NAME, "h1")),
        message="H1 element not visible on the page."
    )

    h1_text = context.driver.find_element(By.TAG_NAME, "h1").text.strip()

    assert h1_text == "Checkout"