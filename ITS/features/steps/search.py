import time
from behave import *
from selenium.webdriver.common.by import By
import re

@given(u'User is on the home page')
def step_impl(context):
    context.driver.get("http://opencart:8080/")
    context.driver.set_window_size(1440, 786)
    time.sleep(1)


@when(u'User types a query into the search')
def step_impl(context):
    context.driver.find_element(By.NAME, "search").click()
    context.driver.find_element(By.NAME, "search").send_keys("mac")
    time.sleep(1)
    
@when(u'User types a query with non existent product into the search')
def step_impl(context):
    context.driver.find_element(By.NAME, "search").click()
    context.driver.find_element(By.NAME, "search").send_keys("ketchup")
    time.sleep(1)


@when(u'User presses search')
def step_impl(context):
    context.driver.find_element(By.CSS_SELECTOR, ".fa-magnifying-glass").click()
    time.sleep(1)


@then(u'Results page is displayed with relevant product')
def step_impl(context):
    # Fetch all product names from the product-list
    product_elements = context.driver.find_elements(By.XPATH, "//div[@id='product-list']//h4/a")

    # Checking if any product name contains 'mac' case-insensitively
    found = any(re.search(r"mac", product_element.text, re.IGNORECASE) for product_element in product_elements)

    assert found


@then(u'A message is displayed indicating no products were found')
def step_impl(context):
    # Find elements that might contain the message
    elements = context.driver.find_elements(By.XPATH, "//body//*[contains(text(), 'no product that matches')]")

    assert len(elements) > 0


@when(u'User clicks on a desired category')
def step_impl(context):
    context.driver.find_element(By.LINK_TEXT, "Tablets").click()
    time.sleep(1)


@then(u'Results page is displayed with relevant products')
def step_impl(context):
    element = context.driver.find_element(By.XPATH, "//div[@id='product-list']//a[contains(text(),'Samsung Galaxy Tab 10.1')]")
    assert element.text == "Samsung Galaxy Tab 10.1"
    

@given(u'User is on the results page')
def step_impl(context):
    context.driver.get("http://opencart:8080/")
    context.driver.set_window_size(1440, 786)
    context.driver.find_element(By.LINK_TEXT, "Tablets").click()
    time.sleep(1)


@when(u'User clicks on a product')
def step_impl(context):
    context.driver.find_element(By.LINK_TEXT, "Samsung Galaxy Tab 10.1").click()
    time.sleep(1)


@then(u'Corresponding product page is displayed')
def step_impl(context):
    assert context.driver.find_element(By.CSS_SELECTOR, "h1").text == "Samsung Galaxy Tab 10.1"


@given(u'User is on a category page displaying multiple products')
def step_impl(context):
    context.driver.get("http://opencart:8080/")
    context.driver.set_window_size(1440, 786)
    context.driver.find_element(By.LINK_TEXT, "Components").click()
    context.driver.find_element(By.LINK_TEXT, "Monitors (2)").click()
    time.sleep(1)


@when(u'User selects an option from the sorting dropdown')
def step_impl(context):
    context.driver.find_element(By.ID, "input-sort").click()
    dropdown = context.driver.find_element(By.ID, "input-sort")
    dropdown.find_element(By.XPATH, "//option[. = 'Name (Z - A)']").click()
    time.sleep(1)


@then(u'Products are reorganized and displayed according to the expected order')
def step_impl(context):
    # Fetch all product names from the product-list
    product_names = [element.text for element in context.driver.find_elements(By.XPATH, "//div[@id='product-list']//h4/a")]

    # Function to check if the list is sorted from Z to A
    def is_sorted_z_to_a(product_list):
        sorted_list = sorted(product_list, reverse=True)
        return sorted_list == product_list

    # Assert that the product names are sorted from Z to A
    assert is_sorted_z_to_a(product_names)