#!/usr/bin/env python3

import asyncio
import inspect
import aiohttp

async def get_urls(urls):
    async with aiohttp.ClientSession() as session:
        tasks = []
        for url in urls:
            task = session.get(url)
            tasks.append(task)
        responses = await asyncio.gather(*[handle_response(task, url) for task, url in zip(tasks, urls)])
        return responses

async def handle_response(task, url):
    try:
        async with task as response:
            return (response.status, url)
    except Exception as e:
        # Get the module name and class name of the exception
        exception_module = inspect.getmodule(e).__name__.split('.')
        exception_name = type(e).__name__
        full_exception_name = f"{exception_module[0]}.{exception_name}"
        return (full_exception_name, url)

if __name__ == '__main__':
    urls = ['https://www.fit.vutbr.cz', 'https://www.szn.cz', 'https://office.com']
    
    # For MS Windows
    # asyncio.set_event_loop_policy(asyncio.WindowsSelectorEventLoopPolicy())

    res = asyncio.run(get_urls(urls))
    assert res == [(200, 'https://www.fit.vutbr.cz'), ('aiohttp.ClientConnectorError', 'https://www.szn.cz'), (200, 'https://office.com')]
    print(res)
