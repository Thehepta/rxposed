package hepta.rxposed.manager.fragment.check;

public class ItemBean {

    String index;
    String msg;
    boolean status;
    public ItemBean(String msg,boolean status) {
        this.msg = msg;
        this.status = status;
    }
    public String getIndex() {
        return index;
    }

    public void setMsg(String msg) {
        this.msg = msg;
    }

    public String getMsg() {
        return msg;
    }

    public void setStatus(boolean status) {
        this.status = status;
    }
}
