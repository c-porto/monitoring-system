import requests as rq

def check_for_valid_res(res: rq.Response) -> bool:
    if res.status_code != 200:
        print(f'Wrong status code: {res.status_code}')
        return False
    if 'application/json' not in res.headers['content-type']:
        print(f'Wrong headers: {res.headers}')
        return False

    return True

def get_measurements_data(endpoint: str) -> list[dict] | None:
    try:
        res: rq.Response = rq.get(endpoint)
        if check_for_valid_res(res):
            return res.json()
        else:
            return None
    except:
        print("Error when requesting data from server")
