from selenium import webdriver
from selenium.webdriver.chrome.options import Options as ChromeOptions
from selenium.webdriver.firefox.options import Options as FirefoxOptions
from selenium.common.exceptions import WebDriverException

BASE_URL = "http://localhost:8080/"

def get_webdriver():
    try:
        chrome_options = ChromeOptions()
        driver = webdriver.Remote(
            command_executor='http://localhost:4444/wd/hub',
            options=chrome_options)
    except WebDriverException:
        firefox_options = FirefoxOptions()
        driver = webdriver.Remote(
            command_executor='http://localhost:4444/wd/hub',
            options=firefox_options)
    driver.implicitly_wait(5)
    return driver

def before_all(context):
	context.driver = get_webdriver()

def after_all(context):
	context.driver.quit()