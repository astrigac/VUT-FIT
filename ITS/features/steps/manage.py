import time
from behave import *
from selenium.webdriver.common.by import By
from selenium.webdriver.common.action_chains import ActionChains
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.common.keys import Keys

@given(u'User is logged in as an admin')
def step_impl(context):
    context.driver.find_element(By.ID, "input-username").click()
    context.driver.find_element(By.ID, "input-username").send_keys("user")
    context.driver.find_element(By.ID, "input-password").click()
    context.driver.find_element(By.ID, "input-password").send_keys("bitnami")
    context.driver.find_element(By.CSS_SELECTOR, ".btn").click()


@given(u'User is on the admin dashboard')
def step_impl(context):
    context.driver.get("http://opencart:8080/administration")
    context.driver.set_window_size(1440, 786)


@when(u'User clicks on the "Products" tab in the drop down menu')
def step_impl(context):
    context.driver.find_element(By.LINK_TEXT, "Catalog").click()
    context.driver.find_element(By.LINK_TEXT, "Products").click()


@then(u'List of products with details is displayed')
def step_impl(context):
    WebDriverWait(context.driver, 10).until(
        EC.visibility_of_element_located((By.TAG_NAME, "h1")),
        message="H1 element not visible on the page."
    )

    h1_text = context.driver.find_element(By.TAG_NAME, "h1").text.strip()

    assert h1_text == "Products"